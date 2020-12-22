#pragma once
#pragma once
#include "camera.h"
#include <shared_mutex>
#include <opencv2/opencv.hpp>
#include "display_opencv.h"
#include <queue>

enum ProcesserWorkMode {
	RGB,
	DEPTH,
	RGB_BODY
};

class Processer
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="displayJoints"></param>
	/// <param name="algrothrim"> 
	///		0：使用openpose检测骨骼点	
	///		1：使用Astra SDK检测骨骼点
	/// </param>
	Processer(bool* displayJoints,int algrothrim);
	~Processer();
	void camera_initialize(std::string license, ProcesserWorkMode mode, int width = 640, int height = 480);
	void detect(bool* isThreadAlive);
	void begin_detect();
	int begin_collect(std::string label);
private:
	void* rgbData;
	std::map<astra::JointType, astra::Vector2i> jointsData;
	//队列中待处理数量
	int numInQueue;
	mutable std::shared_mutex numInQueueMutex;
	bool isThreadAlive;
	//存储骨骼序列连续帧
	std::queue<float*> jointFrameData;
	//socket cummunicate
	std::string messOut;
	bool messIsNew;
	std::string currentLabel;
	float currentScore;
	mutable std::shared_mutex labelMutex;
	//collect data flag
	bool collectMode;
	bool collectBegin;
	std::string collectLabel;

	//astra sdk 相关
	astra::StreamSet* streamSet;
	astra::StreamReader* streamReader;
	MultiFrameListener* listener;
	// open cv 显示类
	Display display;
	//openpose 调用类
	void* jointByOpenpose;
	
	bool* displayJoints;

	void convert_astrargb_to_mat(cv::Mat& outputMat);
	//detect person with astra sdk
	void draw_person_rect(cv::Mat& mat);
	//detect person with openpose  
	int* draw_person_rect(cv::Mat& mat,float* jointsArray);
	//draw skeleton with astra sdk
	void draw_skeleton(cv::Mat& mat);
	void drawLine(cv::Mat& frame, std::map<astra::JointType, astra::Vector2i> &jointsPosition, astra::JointType v1, astra::JointType v2);
	//清空连续骨骼帧队列
	void clearJointFrame();
	//向骨骼帧队列中增加帧
	void addJointFrame(float* jointArray);
	//删除 n 个骨骼帧队列中元素
	void deleteJointFrame(int n);

	//convert astra sdk joints to Openpose Joints
	int convert_joints_astra_to_openpose(float* jointArray);

	//socket
	void* socket_client;

};