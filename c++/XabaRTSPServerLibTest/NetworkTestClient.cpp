#include "pch.h"
#include "NetworkTestClient.h"
#include "ws2tcpip.h"
#include <thread>
#include <Logger\LoggerFactory.h>
using namespace std::chrono_literals;

void NetworkTestClient::Init(std::string host, std::string portNumber){
	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (_socket == INVALID_SOCKET) {
		throw std::runtime_error("socket failed with error: " + WSAGetLastError());
	}

	struct addrinfo hints, * addrInfoInit;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int iResult = getaddrinfo(host.data(), portNumber.c_str(), &hints, &addrInfoInit);
	if (iResult != 0) {
		throw std::runtime_error("getaddrinfo failed with error:" + std::to_string(iResult));
	}

	std::shared_ptr<addrinfo> addrInfo(addrInfoInit, freeaddrinfo);

	WSABUF callerData, calleeData;
	iResult = WSAConnect(_socket, addrInfoInit->ai_addr, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL);
	if (iResult != 0) {
		iResult = WSAGetLastError();
		throw std::runtime_error("WSAConnect failed with error:" + std::to_string(iResult));
	}


}

void NetworkTestClient::Send(std::string content){
	WSABUF buffer;
	buffer.buf = content.data();
	buffer.len = content.length();
	DWORD sent;
	auto result = WSASend(_socket, &buffer, 1, &sent, 0, NULL, NULL);
	if (result != 0) {
		result = WSAGetLastError();
		throw std::runtime_error("WSAConnect failed with error:" + std::to_string(result));
	}

	if (sent != buffer.len) {
		LOGGER->LogInfo("Tcp Client sent failed");
		std::this_thread::sleep_for(100ms);
		Send(content);
	}
}

std::string NetworkTestClient::Receive() {
	WSABUF buffer;

	buffer.buf = new char[1024];
	buffer.len = 1024;
	DWORD given{};
	DWORD flags{};
	auto result = WSARecv(_socket, &buffer, 1, &given, &flags, NULL, NULL);
	if (result != 0) {
		result = WSAGetLastError();
		throw std::runtime_error("WSAConnect failed with error:" + std::to_string(result));
	}

	return std::string(buffer.buf, buffer.buf + given);
}


void NetworkTestClient::Close(){
	closesocket(_socket);
}
