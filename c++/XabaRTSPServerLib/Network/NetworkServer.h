#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <Singleton.h>
#include "BlockingQueue.h"
#include "SocketHandler.h"
#include "NetworkPackage.h"
class NetworkServer {
private:
    BlockingQueue<TCPArrivedNetworkPackage>& _inputqueue = Singleton<BlockingQueue<TCPArrivedNetworkPackage>>();
    BlockingQueue<TCPResponseNetworkPackage>& _outputqueue = Singleton<BlockingQueue<TCPResponseNetworkPackage>>();
    std::atomic_bool _finish;
    std::atomic_bool _terminated;
    HANDLE _completionPort { NULL };
    std::vector<std::thread> _workers;
    std::vector<std::shared_ptr<SocketHandler>> _openPorts;

    void StartWorkers(uint8_t numberOfThreads);
    void Worker();
    void SaveData(const WSABUF& buffer, const DWORD receivedBytes,
        const sockaddr_in& srcIp,
        const std::string& dstIp, int16_t dstPort);
    void CreatePort(std::string_view host, int port);

public:
    void StartServer(std::string_view host, const std::vector<int>& ports, uint8_t numberOfThreads);
    void StopServer();

    bool Stopped();
};

