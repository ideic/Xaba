#pragma once
#include <string>
#include <WinSock2.h>
class NetworkTestClient{
private:
	SOCKET _socket;
public:
	
	void Init(std::string host, std::string port);

	void Send(std::string content);

	std::string Receive();

	void Close();
};

