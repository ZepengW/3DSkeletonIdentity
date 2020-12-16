#pragma once
#pragma once
#include "camera.h"
#include <shared_mutex>
#include <opencv2/opencv.hpp>
#include "display_opencv.h"

enum ProcesserWorkMode {
	COLLECT_RGB_DEPTH,
	COLLECT_RGB_JOINTS_ASTRA_AUTO,
	COLLECT_RGB_JOINTS_OPENPOSE_AUTO,
	DETECT_BY_JOINTS
};

class Processer
{
public:
	Processer();
	~Processer();
	void camera_initialize(std::string license, ProcesserWorkMode mode, int width = 640, int height = 480);
	void detect(bool* isThreadAlive);
	void begin_detect();
private:
	void* rgbData;
	std::map<astra::JointType, astra::Vector2i> jointsData;
	int numInQueue;
	mutable std::shared_mutex numInQueueMutex;
	bool isThreadAlive;

	astra::StreamSet* streamSet;
	astra::StreamReader* streamReader;
	MultiFrameListener* listener;

	Display display;

	void convert_astrargb_to_mat(cv::Mat& outputMat);
};