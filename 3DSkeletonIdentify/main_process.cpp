#include "main_process.h"
#include <astra/astra.hpp>
#include <thread>
#include "joint_by_openpose.h"
#include "calculate_feature.h"
#include "tcp_client.h"

#define WIDTH 640
#define HEIGHT 480
#define MAX_QUEUE 1		//对于视频采集的限制
#define MAX_FRAME 30	//对于骨骼步态信息提取的限制

void camera_fresh(int* numInQueue, mutable std::shared_mutex* mutex, bool* isThreadAlive);
void check_message(std::string* mess, bool* mess_new, std::string* cLabel, float* cScore , bool* isAlive);

Processer::Processer(bool* displayJoints, int algrothrim) :display(WIDTH, HEIGHT)
{
	this->rgbData = (astra::RgbPixel*)malloc(WIDTH * HEIGHT * sizeof(astra::RgbPixel));
	this->jointsData.clear();
	this->numInQueue = 0;
	this->isThreadAlive = true;

	this->streamReader = NULL;
	this->streamSet = NULL;
	this->listener = NULL;
	this->displayJoints = displayJoints;

	this->currentLabel = "";
	this->currentScore = 0.0;
	this->messIsNew = false;
	this->messOut = "";

	this->collectMode = false;
	this->collectBegin = false;
	this->collectLabel = "";
	
	//openpose
	if (0 == algrothrim)
	{
		this->jointByOpenpose = new JointByOpenpose();
		camera_initialize("license", ProcesserWorkMode::RGB, WIDTH, HEIGHT);
	}
	if (1 == algrothrim)
	{
		this->jointByOpenpose = NULL;
		camera_initialize("license", ProcesserWorkMode::RGB_BODY, WIDTH, HEIGHT);
	}
	//socket
	this->socket_client = new TcpClient("127.0.0.1", 1996,&this->messOut,&this->messIsNew);
	//recv listener
	std::thread p = std::thread(check_message, &this->messOut, &this->messIsNew, &this->currentLabel, &this->currentScore,&this->isThreadAlive);
	p.detach();
}

Processer::~Processer()
{
	this->isThreadAlive = false;
	cv::waitKey(1000);	//粗暴地等待子线程结束
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
	//openpose delete
	if (NULL != this->jointByOpenpose)
	{
		delete this->jointByOpenpose;
		this->jointByOpenpose = NULL;
	}

	astra::terminate();
	//data release
	if (NULL != this->rgbData)
		free(this->rgbData);
	this->clearJointFrame();
	//socket release
	if (NULL != this->socket_client)
	{
		delete this->socket_client;
		this->socket_client = NULL;
	}
}

void Processer::camera_initialize(std::string license, ProcesserWorkMode mode, int width, int height)
{
	astra::initialize();

	// license for skeleton
	orbbec_body_tracking_set_license(license.c_str());
	this->streamSet = new astra::StreamSet();
	this->streamReader = new astra::StreamReader(streamSet->create_reader());
	//set stream detect begin
	if (RGB == mode)
	{
		auto mode_rgb = this->streamReader->stream<astra::ColorStream>().mode();
		mode_rgb.set_width(width);
		mode_rgb.set_height(height);
		this->streamReader->stream<astra::ColorStream>().set_mode(mode_rgb);
		this->streamReader->stream<astra::ColorStream>().start();
	}
	else if (DEPTH == mode)
	{
		auto mode_depth = this->streamReader->stream<astra::DepthStream>().mode();
		mode_depth.set_width(width);
		mode_depth.set_height(height);
		this->streamReader->stream<astra::DepthStream>().set_mode(mode_depth);
		this->streamReader->stream<astra::DepthStream>().start();
	}
	else if (RGB_BODY == mode)
	{
		auto mode_rgb = this->streamReader->stream<astra::ColorStream>().mode();
		mode_rgb.set_width(width);
		mode_rgb.set_height(height);
		this->streamReader->stream<astra::ColorStream>().set_mode(mode_rgb);
		this->streamReader->stream<astra::ColorStream>().start();
		this->streamReader->stream<astra::BodyStream>().start();
	}

	this->listener = new MultiFrameListener(this->rgbData, NULL, &this->jointsData, &this->numInQueue, MAX_QUEUE);
	this->streamReader->add_listener(*this->listener);
	//摄像头数据采集数据更新线程
	std::thread p = std::thread(camera_fresh, &this->numInQueue, &this->numInQueueMutex, &this->isThreadAlive);
	p.get_id();
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
		cv::Mat rgbmat = cv::Mat(HEIGHT, WIDTH, CV_8UC3);
		this->numInQueueMutex.lock();
		this->convert_astrargb_to_mat(rgbmat);
		
		//检测骨骼点，标出人体框图
		JointByOpenpose* jointMethod = (JointByOpenpose*)this->jointByOpenpose;
		if (NULL != jointMethod)	//detect with openpose
		{
			float* jointsArray = (float*)malloc(18 * 3 * sizeof(float));
			cv::Mat rgbWithJoints = cv::Mat(HEIGHT, WIDTH, CV_8UC3);
			//detect person/joints
			if (jointMethod->getJointsFromRgb(rgbmat, rgbWithJoints, jointsArray))
			{
				if (*this->displayJoints)
				{
					rgbWithJoints.copyTo(rgbmat);
				}
				//draw rectangle
				int* loc = this->draw_person_rect(rgbmat, jointsArray);
				if (NULL != loc)
				{
					if (this->collectMode && !this->collectBegin)
					{
						this->clearJointFrame();
						this->collectBegin = true;
					}
					this->addJointFrame(jointsArray);

					if (this->jointFrameData.size() > 20 && this->currentScore < 0.7&& !this->collectMode)
					{	
						//帧数大于20 ，之前判断的置信度小于0.7 ，且为检测模式
						std::string labelScore;
						((TcpClient*)this->socket_client)->query_skeleton_label(this->jointFrameData, this->messOut, this->messIsNew);
						this->deleteJointFrame(10);
					}
					else if(this->jointFrameData.size()==MAX_FRAME&&this->collectMode&&this->collectBegin)
					{
						//上传数据
						((TcpClient*)this->socket_client)->collect_skeleton_label(this->jointFrameData, this->collectLabel, this->messOut, this->messIsNew);
						this->clearJointFrame();
						this->collectMode = false;
						this->collectBegin = false;
					}
					//convert label and score to string , and show
					std::string label;
					if (this->currentScore > 0.5)
						label = this->currentLabel + ":" + std::to_string(this->currentScore);
					else if (this->currentScore == 0.0)
						label = "computing";
					else
						label = "unknow";
					if(!this->collectMode)	//don't show label in collect mode 
						putText(rgbmat, label, cv::Point(loc[0], loc[2]), cv::FONT_HERSHEY_COMPLEX, 0.7,
							cv::Scalar(0, 0, 255), 2, 8, 0);
					free(loc);
					loc = NULL;
				}
			}
			else //didn't detect person joints 
			{
				this->currentLabel = "";
				this->currentScore = 0.0;
				if (this->jointFrameData.size() >= 15)
				{
					//上传数据
					((TcpClient*)this->socket_client)->collect_skeleton_label(this->jointFrameData, this->collectLabel, this->messOut, this->messIsNew);
					this->clearJointFrame();
					this->collectMode = false;
					this->collectBegin = false;
				}
				else
				{//无效数据

				}
			}
		}
		else			//detect with astra sdk
		{
			if (!this->jointsData.empty())
			{
				this->draw_person_rect(rgbmat);
				//display joints
				if (NULL != this->displayJoints
					&& *this->displayJoints)
					this->draw_skeleton(rgbmat);
			}
			else
			{
				this->clearJointFrame();
			}
		}	
		this->numInQueue -= 1;
		this->numInQueueMutex.unlock();
		display.display_rgb_frame(rgbmat);
	}
}

void Processer::begin_detect()
{
	std::thread p = std::thread(&Processer::detect,this,&this->isThreadAlive);
	p.detach();
}

int Processer::begin_collect(std::string label)
{
	this->collectMode = true;
	this->collectBegin = false;
	this->collectLabel = label;
	return 0;
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

	cv::Mat mat = cv::Mat(HEIGHT, WIDTH, CV_8UC3, (uint8_t*)data);
	mat.copyTo(outputMat);
}

void Processer::draw_person_rect(cv::Mat& mat)
{
	int x_l=WIDTH, x_h=0, y_l=HEIGHT, y_h = 0;
	for (auto jointData : this->jointsData)
	{
		int x = WIDTH - jointData.second.x;
		int y = jointData.second.y;
		x_l = MIN(x, x_l);
		x_h = MAX(x, x_h);
		y_l = MIN(y, y_l);
		y_h = MAX(y, y_h);
	}
	cv::Rect rect(x_l, y_l, x_h - x_l, y_h - y_l);
	cv::rectangle(mat, rect, cv::Scalar(0, 255, 0), 3, cv::LINE_8, 0);
}

int* Processer::draw_person_rect(cv::Mat& mat, float* jointsArray)
{
	if (NULL == jointsArray)
		return NULL;
	int x_l = WIDTH, x_h = 0, y_l = HEIGHT, y_h = 0;
	for (int i = 0; i < 18; i++)
	{
		if (0 == jointsArray[3 * i + 2])
			continue;
		int x = jointsArray[3 * i];
		int y = jointsArray[3 * i+1];
		x_l = MIN(x, x_l);
		x_h = MAX(x, x_h);
		y_l = MIN(y, y_l);
		y_h = MAX(y, y_h);
	}
	cv::Rect rect(x_l, y_l, x_h - x_l, y_h - y_l);
	cv::rectangle(mat, rect, cv::Scalar(0, 255, 0), 3, cv::LINE_8, 0);
	int* r = (int*)malloc(4 * sizeof(int));
	r[0] = x_l;
	r[1] = x_h;
	r[2] = y_l;
	r[3] = y_h;
	return r;
}

void Processer::draw_skeleton(cv::Mat& mat)
{
	drawLine(mat, this->jointsData, astra::JointType::Head, astra::JointType::Neck);
	drawLine(mat, this->jointsData, astra::JointType::Neck, astra::JointType::ShoulderSpine);

	drawLine(mat, this->jointsData, astra::JointType::ShoulderSpine, astra::JointType::LeftShoulder);
	drawLine(mat, this->jointsData, astra::JointType::LeftShoulder, astra::JointType::LeftElbow);
	drawLine(mat, this->jointsData, astra::JointType::LeftElbow, astra::JointType::LeftWrist);
	drawLine(mat, this->jointsData, astra::JointType::LeftWrist, astra::JointType::LeftHand);

	drawLine(mat, this->jointsData, astra::JointType::ShoulderSpine, astra::JointType::RightShoulder);
	drawLine(mat, this->jointsData, astra::JointType::RightShoulder, astra::JointType::RightElbow);
	drawLine(mat, this->jointsData, astra::JointType::RightElbow, astra::JointType::RightWrist);
	drawLine(mat, this->jointsData, astra::JointType::RightWrist, astra::JointType::RightHand);

	drawLine(mat, this->jointsData, astra::JointType::ShoulderSpine, astra::JointType::MidSpine);
	drawLine(mat, this->jointsData, astra::JointType::MidSpine, astra::JointType::BaseSpine);

	drawLine(mat, this->jointsData, astra::JointType::BaseSpine, astra::JointType::LeftHip);
	drawLine(mat, this->jointsData, astra::JointType::LeftHip, astra::JointType::LeftKnee);
	drawLine(mat, this->jointsData, astra::JointType::LeftKnee, astra::JointType::LeftFoot);

	drawLine(mat, this->jointsData, astra::JointType::BaseSpine, astra::JointType::RightHip);
	drawLine(mat, this->jointsData, astra::JointType::RightHip, astra::JointType::RightKnee);
	drawLine(mat, this->jointsData, astra::JointType::RightKnee, astra::JointType::RightFoot);
	cv::Scalar color_point = cv::Scalar(0, 0, 255);
	for (auto jointData : this->jointsData)
	{
		cv::Point p = cv::Point(WIDTH-jointData.second.x, jointData.second.y);
		circle(mat, p, 10, color_point, -1);
	}
}


void Processer::drawLine(cv::Mat& mat, std::map<astra::JointType, astra::Vector2i> jointsPosition, astra::JointType v1, astra::JointType v2)
{
	cv::Scalar color_line = cv::Scalar(255, 0, 0);
	if (0 == jointsPosition.count(v1) ||
		0 == jointsPosition.count(v2))
		return;
	cv::Point p1 = cv::Point(WIDTH-jointsPosition[v1].x, jointsPosition[v1].y);
	cv::Point p2 = cv::Point(WIDTH-jointsPosition[v2].x, jointsPosition[v2].y);
	line(mat, p1, p2, color_line, 5);
}

void Processer::clearJointFrame()
{
	while (!this->jointFrameData.empty())
	{
		if (this->jointFrameData.front() != NULL)
		{
			free(this->jointFrameData.front());
		}
		this->jointFrameData.pop();
	}
}

void Processer::addJointFrame(float* jointArray)
{
	if (NULL == jointArray)
	{
		this->clearJointFrame();
	}
	this->jointFrameData.push(jointArray);
	while (this->jointFrameData.size() > MAX_FRAME)
	{
		if (NULL != this->jointFrameData.front())
			free(this->jointFrameData.front());
		this->jointFrameData.pop();
	}
}

void Processer::deleteJointFrame(int n)
{
	int count = 0;
	while (!this->jointFrameData.empty()&&
		count < n)
	{
		if (this->jointFrameData.front() != NULL)
		{
			free(this->jointFrameData.front());
		}
		this->jointFrameData.pop();
		count++;
	}
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

void check_message(std::string* mess, bool* mess_new, std::string* cLabel, float* cScore, bool* isAlive)
{
	while (*isAlive)
	{
		if (NULL == mess ||
			NULL == mess_new ||
			NULL == cLabel ||
			NULL == cScore )
			return;
		if (!*mess_new)
			continue;
		const char* mess_c = mess->c_str();
		char mess_type = mess_c[0];
		
		if ('L' == mess_type)	// 返回的是标签
		{
			// get label
			std::string::size_type posL = mess->find(':');
			if (posL == mess->npos)
				return;
			*cLabel = mess->substr(1, posL - 1);
			// get score
			std::string::size_type posS = mess->find(':', posL);
			if (posS == mess->npos)
				return;
			*cScore = atof(mess->substr(posL + 1, posS - posL - 1).c_str());
		}
	}
}
