#pragma once
#include <string>
#include <WinSock2.h>
#include <tuple>
class NetworkUtility
{
public:
	static std::tuple<std::string, uint16_t> FromInAddrToStringIpPort(const sockaddr_in& ip);

	static sockaddr_in FromStringIpPortToInAddr(const std::string & ip, uint16_t port);
};

