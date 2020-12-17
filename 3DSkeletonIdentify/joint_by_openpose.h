#pragma once
#pragma once
// Third-party dependencies
#include <opencv2/opencv.hpp>
// Command-line user interface
#define OPENPOSE_FLAGS_DISABLE_PRODUCER
#define OPENPOSE_FLAGS_DISABLE_DISPLAY

// OpenPose dependencies
#include <openpose/headers.hpp>


class JointByOpenpose
{
public:
	JointByOpenpose();
	bool getJointsFromRgb(cv::Mat frame, cv::Mat& frameOut, float* jointsArrayOutput);
private:
	op::Wrapper opWrapper{ op::ThreadManagerMode::Asynchronous };
};