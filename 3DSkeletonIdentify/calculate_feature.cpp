#include "calculate_feature.h"
#include <fstream>
#include <Windows.h>

#define DATASET_PATH "./dataset"

int write_joint_file(std::queue<float*> jointsFrameArray, int frameLen, std::string outputPath);
void calculate_feature(std::string inputPath);
void get_label(std::string inputPath, std::string& id);

BOOL system_hide(std::string command);

void get_joints_id(std::queue<float *> jointsFrameArray, int frameLen, std::string& id)
{
	if (0 == jointsFrameArray.size())
	{
		id = "ERROR";
		return;
	}
	int ret = write_joint_file(jointsFrameArray, frameLen, "./jointFrames_tmp.txt");
	if (-1 == ret)
	{
		id = "ERROR";
		return;
	}
	calculate_feature("./jointFrames_tmp.txt");
	get_label("./result.txt", id);
}

int write_joint_file(std::queue<float*> jointsFrameArray, int frameLen, std::string outputPath)
{
	std::ofstream ofile;
	ofile.open(outputPath);
	if (!ofile.is_open())
		return -1;
	ofile.clear();
	
	while(0!=jointsFrameArray.size())
	{
		std::string frameStr;
		float* jointsArray = jointsFrameArray.front();
		for (int j = 0; j < 18 * 3; j++)
		{
			frameStr.append(std::to_string(jointsArray[j])+',');
		}
		jointsFrameArray.pop();
		ofile << frameStr << std::endl;
	}
	ofile.close();
	return 0;
}

void calculate_feature(std::string inputPath)
{
	std::string command_rd = "python ./api/calculate_feature.py " + inputPath+" "+DATASET_PATH+" "+"./result.txt";
	system_hide(command_rd);
}

void get_label(std::string inputPath, std::string& id)
{
	std::ifstream ifile;
	ifile.open(inputPath);
	if (ifile.is_open())
	{
		std::getline(ifile, id);
		return;
	}
	id = "ERROR";
}
BOOL system_hide(std::string command)
{
	SECURITY_ATTRIBUTES   sa;
	HANDLE   hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return   FALSE;
	}

	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN  
	std::wstring widstr;
	widstr = std::wstring(command.begin(), command.end());
	if (!CreateProcess(NULL, (LPWSTR)widstr.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		return   FALSE;
	}
	CloseHandle(hWrite);

	char   buffer[4096] = { 0 };
	DWORD   bytesRead;
	while (true)
	{
		memset(buffer, 0, strlen(buffer));
		if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)
			break;
		//buffer中就是执行的结果，可以保存到文本，也可以直接输出     
		//printf(buffer);//这行注释掉就可以了     
		Sleep(100);
	}
	return   TRUE;
}