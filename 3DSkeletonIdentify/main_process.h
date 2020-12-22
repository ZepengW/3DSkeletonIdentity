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
	///		0��ʹ��openpose��������	
	///		1��ʹ��Astra SDK��������
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
	//�����д���������
	int numInQueue;
	mutable std::shared_mutex numInQueueMutex;
	bool isThreadAlive;
	//�洢������������֡
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

	//astra sdk ���
	astra::StreamSet* streamSet;
	astra::StreamReader* streamReader;
	MultiFrameListener* listener;
	// open cv ��ʾ��
	Display display;
	//openpose ������
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
	//�����������֡����
	void clearJointFrame();
	//�����֡����������֡
	void addJointFrame(float* jointArray);
	//ɾ�� n ������֡������Ԫ��
	void deleteJointFrame(int n);

	//convert astra sdk joints to Openpose Joints
	int convert_joints_astra_to_openpose(float* jointArray);

	//socket
	void* socket_client;

};