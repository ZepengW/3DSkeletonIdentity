#pragma once
#include <opencv2/opencv.hpp>

class Display
{
public:
	Display(int width, int height);
	~Display();
	void display_rgb_frame(cv::Mat frame);
private:
	std::string cvWinName;
	int width;
	int height;

};
