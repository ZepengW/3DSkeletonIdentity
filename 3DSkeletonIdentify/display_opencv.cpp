#include "display_opencv.h"
#include <Windows.h>

using namespace cv;

Display::Display(int width, int height)
{
	this->cvWinName = "Rgb";
	namedWindow(this->cvWinName);
	//设置初始化封面
	Mat front = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	putText(front, "Waiting Signal...", cv::Point(0, height / 2), FONT_HERSHEY_COMPLEX, 1,
		Scalar(0, 0, 255), 1, 8, 0);
	imshow(this->cvWinName, front);
	cv::waitKey(10);
}

Display::~Display()
{
}

void Display::display_rgb_frame(Mat frame)
{
	imshow(this->cvWinName, frame);
	waitKey(10);
}
