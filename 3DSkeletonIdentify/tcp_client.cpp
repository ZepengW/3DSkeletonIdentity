#include "tcp_client.h"
#include <WS2tcpip.h>
#include <thread>


#define MAX_SEND 10000


void recv_socket(SOCKET s, std::string* message, bool* isNew);

TcpClient::TcpClient(const char* servIp, int servPort, std::string* message, bool* isNew)
{
	WSAStartup(MAKEWORD(1, 1), &this->wsa);
	this->servAddr.sin_family = AF_INET;
	this->servAddr.sin_port = htons(servPort);
	inet_pton(AF_INET, servIp, &this->servAddr.sin_addr);
	this->socketClient = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(this->socketClient, (SOCKADDR*)&this->servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		WSACleanup();
	}
	else
	{

	}
	err = 0;
	state = 0;

	std::thread p = std::thread(recv_socket, this->socketClient, message, isNew);
	p.detach();

}

TcpClient::~TcpClient()
{
	WSACleanup();
}

int TcpClient::connect_socket()
{
	if (connect(this->socketClient, (struct sockaddr*)&this->servAddr, sizeof(this->servAddr)) == INVALID_SOCKET) {
		err = 3;
		return 0;
	}
	state = 1;
	return 1;
}

int TcpClient::query_skeleton_label(std::queue<float*> skeletonFrames, std::string& mess, bool& mess_new)
{
	std::string sendData;
	while (0 != skeletonFrames.size())
	{
		float* skeletonArray = skeletonFrames.front();
		skeletonFrames.pop();
		if (NULL != skeletonArray)
		{
			for (int i = 0; i < 18 * 3 - 1; i++)
			{
				sendData += std::to_string(*skeletonArray) + ",";
				skeletonArray++;
			}
			sendData += std::to_string(*skeletonArray) + ";";
		}
	}
	if (sendData.empty())
		return -1;
	sendData = "Q" + sendData;
	send(this->socketClient, sendData.c_str(), strlen(sendData.c_str()), 0);
	return 0;
}

int TcpClient::collect_skeleton_label(std::queue<float*> skeletonFrames, std::string label, std::string& mess, bool& mess_new)
{
	std::string sendData;
	while (0 != skeletonFrames.size())
	{
		float* skeletonArray = skeletonFrames.front();
		skeletonFrames.pop();
		if (NULL != skeletonArray)
		{
			for (int i = 0; i < 18 * 3 - 1; i++)
			{
				sendData += std::to_string(*skeletonArray) + ",";
				skeletonArray++;
			}
			sendData += std::to_string(*skeletonArray) + ";";
		}
	}
	if (sendData.empty())
		return -1;
	sendData = "L"+label+"S" + sendData;
	send(this->socketClient, sendData.c_str(), strlen(sendData.c_str()), 0);
	return 0;
}

void recv_socket(SOCKET s, std::string* message, bool* isNew)
{
	char data[1024];
	memset(data, 0, 1024);
	while (1)
	{
		int end = recv(s, data, 1024, 0);
		if (-1 == end)
			continue;
		if (0 == end)
			return;
		data[end] = '\0';
		*message = std::string(data);
		*isNew = true;
	}
}