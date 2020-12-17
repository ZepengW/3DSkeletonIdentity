#pragma once
//调用python工程 进行骨骼特征提取和匹配算法实现

class JointMatch
{
public:
	JointMatch();
	~JointMatch();
private:
	void* pModule;
	
};