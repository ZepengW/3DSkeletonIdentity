// This file is part of the Orbbec Astra SDK [https://orbbec3d.com]
// Copyright (c) 2015-2017 Orbbec 3D
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Be excellent to each other.
#include "camera.h"



MultiFrameListener::MultiFrameListener(void* rgbData, void* depthData, std::map<astra::JointType, astra::Vector2i>* jointData, int* numInQueue, int queueMax)
{
	this->rgbDataOutput = (astra::RgbPixel*)rgbData;
	this->depthDataOutput = (astra::int16_t*)depthData;
	this->jointDataOutput = jointData;
	this->numInQueue = numInQueue;
	this->queueMax = queueMax;
}

MultiFrameListener::~MultiFrameListener()
{
}

void MultiFrameListener::get_joint_data(const astra::BodyFrame& bodyFrame)
{
	if (NULL == this->jointDataOutput)
		return;
	if (!bodyFrame.is_valid()
		|| bodyFrame.bodies().empty())
	{
		this->jointDataOutput->clear();
		return;
	}
	for (auto& body : bodyFrame.bodies())
	{
		// only detect one preson for now
		for (auto& joint : body.joints())
		{
			astra::JointType type = joint.type();
			astra::JointStatus status = joint.status();
			if (status != astra::JointStatus::Tracked)
				continue;
			const auto& pose = joint.depth_position();
			(*this->jointDataOutput)[type] = astra::Vector2i(pose.x, pose.y);
		}
	}
}

void MultiFrameListener::on_frame_ready(astra::StreamReader& reader,
	astra::Frame& frame)
{
	int i = 0;
	if (NULL != this->rgbDataOutput
		&& frame.get<astra::ColorFrame>().is_valid())
	{
		const astra::ColorFrame colorFrame = frame.get<astra::ColorFrame>();
		colorFrame.copy_to(this->rgbDataOutput);
		i = 1;
	}
	if (NULL != this->depthDataOutput
		&& frame.get<astra::DepthFrame>().is_valid())
	{
		const astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();
		depthFrame.copy_to(this->depthDataOutput);
		i = 1;
	}
	if (NULL != this->jointDataOutput
		&& frame.get<astra::BodyFrame>().is_valid())
	{
		const astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();
		this->get_joint_data(bodyFrame);
		i = 1;
	}
	*this->numInQueue += i;

}