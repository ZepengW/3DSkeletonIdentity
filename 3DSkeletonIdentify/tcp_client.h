#pragma once
#include <WinSock2.h>
#include <queue>
#include <string>
#include <shared_mutex>

class TcpClient {
private:
	SOCKET socketClient;
	SOCKADDR_IN servAddr;
	int err, state;
	WSADATA wsa;
	int recvThreadNum;
	mutable std::shared_mutex recvThreadNumMutex;
public:
	TcpClient(const char* servIp, int servPort, std::string* message, bool* isNew);
	~TcpClient();
	int connect_socket();
	//���͹������ݣ�����label and score
	int query_skeleton_label(std::queue<float*> skeletonFrames, std::string& mess, bool& mess_new);
	//���͹������ݣ������浽���ݿ�
	int collect_skeleton_label(std::queue<float*> skeletonFrames, std::string label, std::string& mess, bool& mess_new);
};