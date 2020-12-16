#include "main_process.h"
#include <astra/astra.hpp>
#include <thread>
#define WIDTH 640
#define HEIGHT 480
#define MAX_QUEUE 1 

void camera_fresh(int* numInQueue, mutable std::shared_mutex* mutex, bool* isThreadAlive);

Processer::Processer() :display(WIDTH, HEIGHT)
{
	this->rgbData = (astra::RgbPixel*)malloc(WIDTH * HEIGHT * sizeof(astra::RgbPixel));
	this->jointsData.clear();
	this->numInQueue = 0;
	this->isThreadAlive = true;

	this->streamReader = NULL;
	this->streamSet = NULL;
	this->listener = NULL;
	camera_initialize("license", ProcesserWorkMode::DETECT_BY_JOINTS, WIDTH, HEIGHT);
}

Processer::~Processer()
{
	this->isThreadAlive = false;
	//astra camera terminate
	if (NULL != this->streamReader)
	{
		if (NULL != this->listener)
		{
			this->streamReader->remove_listener(*this->listener);
			delete this->listener;
			this->listener = NULL;
		}
		delete this->streamReader;
		this->streamReader = NULL;
	}
	if (NULL != this->streamSet)
	{
		delete this->streamSet;
		this->streamSet = NULL;
	}

	astra::terminate();
	//data release
	if (NULL != this->rgbData)
		free(this->rgbData);
}

void Processer::camera_initialize(std::string license, ProcesserWorkMode mode, int width, int height)
{
	astra::initialize();

	// license for skeleton
	orbbec_body_tracking_set_license(license.c_str());
	this->streamSet = new astra::StreamSet();
	this->streamReader = new astra::StreamReader(streamSet->create_reader());
	//set stream detect begin
	auto mode_depth = this->streamReader->stream<astra::DepthStream>().mode();
	mode_depth.set_width(width);
	mode_depth.set_height(height);
	this->streamReader->stream<astra::DepthStream>().set_mode(mode_depth);
	this->streamReader->stream<astra::DepthStream>().start();
	auto mode_rgb = this->streamReader->stream<astra::ColorStream>().mode();
	mode_rgb.set_width(width);
	mode_rgb.set_height(height);
	this->streamReader->stream<astra::ColorStream>().set_mode(mode_rgb);
	this->streamReader->stream<astra::ColorStream>().start();
	this->streamReader->stream<astra::BodyStream>().start();

	this->listener = new MultiFrameListener(this->rgbData, NULL, &this->jointsData, &this->numInQueue, MAX_QUEUE);
	this->streamReader->add_listener(*this->listener);
	//摄像头数据采集数据更新线程
	std::thread p = std::thread(camera_fresh, &this->numInQueue, &this->numInQueueMutex, &this->isThreadAlive);
	p.detach();

}

void Processer::detect(bool* isThreadAlive)
{
	while (*isThreadAlive)
	{
		this->numInQueueMutex.lock_shared();
		int num = this->numInQueue;
		this->numInQueueMutex.unlock_shared();
		if (num <= 0)
			continue;
		//convert astra rgb stream to cv::Mat
		cv::Mat rgbFrame = cv::Mat(HEIGHT, WIDTH, CV_8UC3);
		this->numInQueueMutex.lock();
		this->convert_astrargb_to_mat(rgbFrame);
		this->numInQueue -= 1;
		this->numInQueueMutex.unlock();
		display.display_rgb_frame(rgbFrame);
	}
}

void Processer::begin_detect()
{
	std::thread p = std::thread(&Processer::detect,this,&this->isThreadAlive);
	p.detach();
}

void Processer::convert_astrargb_to_mat(cv::Mat& outputMat)
{
	uint8_t* data = (uint8_t*)malloc(WIDTH * HEIGHT * 3 * sizeof(uint8_t));
	for (size_t j = 0; j < HEIGHT; j++)
	{
		for (size_t i = 0; i < WIDTH; i++)
		{
			data[3 * (i + WIDTH * j)] = ((astra::RgbPixel*)this->rgbData)[(WIDTH - i + WIDTH * j)].b;
			data[3 * (i + WIDTH * j) + 1] = ((astra::RgbPixel*)this->rgbData)[(WIDTH - i + WIDTH * j)].g;
			data[3 * (i + WIDTH * j) + 2] = ((astra::RgbPixel*)this->rgbData)[(WIDTH - i + WIDTH * j)].r;
		}
	}

	cv::Mat frame = cv::Mat(HEIGHT, WIDTH, CV_8UC3, (uint8_t*)data);
	frame.copyTo(outputMat);
}

void camera_fresh(int* numInQueue, mutable std::shared_mutex* mutex, bool* isThreadAlive)
{
	while (*isThreadAlive)
	{
		mutex->lock();
		if (*numInQueue < MAX_QUEUE)
			astra_update();
		mutex->unlock();
	}
}
