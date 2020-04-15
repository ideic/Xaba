#include "pch.h"
#include "NetworkServer.h"
#include <stdexcept>
#include <Logger\LoggerFactory.h>
#include <algorithm>
#include "SocketOverlappedContext.h"
#include "SocketHandler.h"
#include "NetworkPackage.h"
#include <mswsock.h>
void NetworkServer::StartWorkers(uint8_t numberOfThreads) {
    for (int i = 0; i < numberOfThreads; ++i) {
        _workers.emplace_back([&]() { Worker(); });
    }
}
//std::shared_ptr<SocketOverlappedContext> Ctx;
void NetworkServer::Worker() {
    while (!_finish) {

        DWORD numberOfBytes = 0;
        uint64_t completionKey = 0;
        LPOVERLAPPED ctx = 0;

        auto ioSucceeds = GetQueuedCompletionStatus(_completionPort,
            &numberOfBytes,
            &completionKey,
            &ctx,
            5'000 //  dwMilliseconds
        );

        if (ioSucceeds) {

            auto overlappedContext = (SocketOverlappedContext*)ctx;
            switch (overlappedContext->State) {
                case OIOMode::Accept: {
                    auto iResult = setsockopt(overlappedContext->AcceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&overlappedContext->ListenSocket, sizeof(overlappedContext->ListenSocket));
                    if (iResult != 0) {
                        iResult = WSAGetLastError();
                        throw std::runtime_error("Cannot setsockopt. Error:" + iResult);
                    }

                    if (numberOfBytes > 0) {
                        SaveData(overlappedContext->Buffer, numberOfBytes,
                            overlappedContext->SrcIp,
                            overlappedContext->DstIp, overlappedContext->DstPort);
                        overlappedContext->ReceivedBytes = 0;
                    }
                    
                    overlappedContext->State = OIOMode::Read;

                    int iresult = WSARecvFrom(
                        overlappedContext->AcceptSocket, &overlappedContext->Buffer, 1,
                        &overlappedContext->ReceivedBytes, &overlappedContext->Flags,
                        (sockaddr*)&overlappedContext->SrcIp,
                        &overlappedContext->SrcIpLength, overlappedContext, NULL);

                    if (iresult != 0) {
                        iresult = WSAGetLastError();
                        if (iresult != WSA_IO_PENDING) {
                            LOGGER->LogWarning("NetworkServer IOCP 2nd WSARecvFrom init failed with Code:" + iresult);
                            // socket closed;
                            iresult = 0;
                        }
                    }

                    try {
                        auto acceptsocket = std::make_shared<SocketHandler>();
                        acceptsocket->CreateAcceptSocket(overlappedContext, _completionPort);

                        _openPorts.push_back(acceptsocket);
                    }
                    catch (const std::exception& e) {
                        LOGGER->LogError(e, "Create socker error on host:" + overlappedContext->DstIp + "port:" + std::to_string(overlappedContext->DstPort));
                        continue;
                    }

                    break;

                }
                case OIOMode::Read: {
                    if (numberOfBytes == 0) continue;
                    SaveData(overlappedContext->Buffer, numberOfBytes,
                        overlappedContext->SrcIp,
                        overlappedContext->DstIp, overlappedContext->DstPort);

                    int iresult = WSARecvFrom(
                        overlappedContext->AcceptSocket, &overlappedContext->Buffer, 1,
                        &overlappedContext->ReceivedBytes, &overlappedContext->Flags,
                        (sockaddr*)&overlappedContext->SrcIp,
                        &overlappedContext->SrcIpLength, overlappedContext, NULL);

                    if (iresult != 0) {
                        iresult = WSAGetLastError();
                        if (iresult != WSA_IO_PENDING) {
                            LOGGER->LogWarning("NetworkServer IOCP 2nd WSARecvFrom init failed with Code:" + iresult);
                            // socket closed;
                            iresult = 0;
                        }
                    }

                }
            }

        }
        else {
            auto iResult = WSAGetLastError();

            if (iResult != WAIT_TIMEOUT) {
                LOGGER->LogWarning("NetworkServer IOCP GetQueuedCompletionStatus failed with Code:" + iResult);
                // Init Receive ?
            }
        }
    }
    _terminated = true;
}

void NetworkServer::StartServer(std::string_view host,
                                const std::vector<int> &ports,
                                uint8_t numberOfThreads) {
    _terminated = false;
    WSADATA wsaData;
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult));
    }

    _finish = false;

  _completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

  if (!_completionPort) {
    throw std::runtime_error("IO Completion port create failed with error: " +  GetLastError());
  }
  
  LOGGER->LogInfo("Start Listening");

  std::for_each(ports.begin(), ports.end(),[this, &host](const int port) { CreatePort(host, port); });

  LOGGER->LogInfo("Start NetworkServer Worker threads:" + std::to_string(numberOfThreads));

  StartWorkers(numberOfThreads);
}

void NetworkServer::CreatePort(std::string_view host, int port)
{
    try {
        std::shared_ptr<SocketHandler> listensocket = std::make_shared<SocketHandler>();
        listensocket->CreateListenSocket(host, port, _completionPort);

        _openPorts.push_back(listensocket);

        auto acceptsocket = std::make_shared<SocketHandler>();
        acceptsocket->CreateAcceptSocket(listensocket->Ctx.get(), _completionPort);

        _openPorts.push_back(acceptsocket);
    }
    catch (const std::exception& e) {
        LOGGER->LogError(e, "Create socker error on host:" + std::string(host) + "port:" + std::to_string(port));
        return;
    }
}

void NetworkServer::StopServer() {
  _finish = true;
  LOGGER->LogInfo("NetworkServer Stop Workers");

  for (auto &worker : _workers) {
    worker.join();
  };

  LOGGER->LogInfo("NetworkServer Clear OpenPorts");
  _openPorts.clear();

  LOGGER->LogInfo("NetworkServer Clear Workers");
  _workers.clear();

  WSACleanup();
}

bool NetworkServer::Stopped(){
    return _terminated;
}


void NetworkServer::SaveData(const WSABUF &buffer, const DWORD receivedBytes,
                             const sockaddr_in &srcIp, 
                             const std::string &dstIp, int16_t dstPort) {
  // wchar_t ip[INET_ADDRSTRLEN];
  // InetNtop(from.sin_family, &from.sin_addr, ip, INET_ADDRSTRLEN);

  TCPArrivedNetworkPackage packet;
  packet.buffer.clear();
  packet.buffer.insert(packet.buffer.end(), buffer.buf, buffer.buf + receivedBytes);
  packet.SetSrc(srcIp);

  packet.dstIp = dstIp;
  packet.dstPort = dstPort;

  packet.rxTimeSec = std::chrono::system_clock::now();

  _inputqueue.Push(std::move(packet));
}