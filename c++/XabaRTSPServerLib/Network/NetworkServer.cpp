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
        _listeners.emplace_back([&]() { Listener(); });
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
                            overlappedContext->DstIp,
                            overlappedContext->AcceptSocket
                        );
                        overlappedContext->ReceivedBytes = 0;
                    }
                    
                    overlappedContext->State = OIOMode::Read;

                    int iresult = WSARecv(
                        overlappedContext->AcceptSocket, &overlappedContext->Buffer, 1,
                        &overlappedContext->ReceivedBytes, &overlappedContext->Flags,
                     /*   (sockaddr*)&overlappedContext->SrcIp,
                        &overlappedContext->SrcIpLength,*/ overlappedContext, NULL);

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
                        LOGGER->LogError(e, "Create socker error on host:" + std::get<0>(NetworkUtility::FromInAddrToStringIpPort(overlappedContext->DstIp)) + "port:" + std::to_string(std::get<1>(NetworkUtility::FromInAddrToStringIpPort(overlappedContext->DstIp))));
                        continue;
                    }

                    break;

                }
                case OIOMode::Read: {
                    if (numberOfBytes == 0) continue;
                    SaveData(overlappedContext->Buffer, numberOfBytes,
                        overlappedContext->SrcIp,
                        overlappedContext->DstIp,
                        overlappedContext->AcceptSocket
                    );

                    int iresult = WSARecv(
                        overlappedContext->AcceptSocket, &overlappedContext->Buffer, 1,
                        &overlappedContext->ReceivedBytes, &overlappedContext->Flags,
                        /*(sockaddr*)&overlappedContext->SrcIp,
                        &overlappedContext->SrcIpLength,*/ overlappedContext, NULL);

                    if (iresult != 0) {
                        iresult = WSAGetLastError();
                        if (iresult != WSA_IO_PENDING) {
                            LOGGER->LogWarning("NetworkServer IOCP 2nd WSARecvFrom init failed with Code:" + iresult);
                            // socket closed;
                            iresult = 0;
                        }
                    }

                }
                case OIOMode::Send: {
                    // int iresult = WSASend(
                    //    overlappedContext->AcceptSocket, &overlappedContext->Buffer, 1,
                    //    &overlappedContext->ReceivedBytes, overlappedContext->Flags,
                    //    overlappedContext, NULL);

                    //if (iresult != 0) {
                    //    iresult = WSAGetLastError();
                    //    if (iresult != WSA_IO_PENDING) {
                    //        LOGGER->LogWarning("NetworkServer WSASend failed with Code:" + iresult);
                    //        // socket closed;
                    //        iresult = 0;
                    //    }
                    //}

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

void NetworkServer::Listener(){
    while (!_finish) {
        auto message = _outputqueue.Pop();
        if (!message) _finish = true;
        if (_finish) break;

        std::shared_ptr<SocketOverlappedContext> Ctx = std::make_shared<SocketOverlappedContext>();
        Ctx->ResetBuffer();
        std::copy(message.value().buffer.begin(), message.value().buffer.end(), Ctx->Buffer.buf);
        Ctx->Buffer.len = message.value().buffer.size();
        Ctx->AcceptSocket = message.value().socket;
        Ctx->State = OIOMode::Send;
        _listenerCtxs.push_back(Ctx);
        int iresult = WSASend(Ctx->AcceptSocket,  &Ctx->Buffer, 1, &Ctx->ReceivedBytes, Ctx->Flags, Ctx.get(), NULL);
        if (iresult != 0) {
            iresult = WSAGetLastError();
            if (iresult != WSA_IO_PENDING) {
                LOGGER->LogWarning("WSASend failed with Code:" + iresult);
                // socket closed;
                iresult = 0;
            }
        }
    }
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

  LOGGER->LogInfo("Terminate queues");
  _outputqueue.Terminate();
  _inputqueue.Terminate();

  for (auto &worker : _workers) {
    worker.join();
  };

  for (auto& worker : _listeners) {
      worker.join();
  };

  LOGGER->LogInfo("NetworkServer Clear OpenPorts");
  _openPorts.clear();

  LOGGER->LogInfo("NetworkServer Clear ListenContexts");
  _listenerCtxs.clear();

  LOGGER->LogInfo("NetworkServer Clear Workers");
  _workers.clear();

  WSACleanup();
}

bool NetworkServer::Stopped(){
    return _terminated;
}


void NetworkServer::SaveData(const WSABUF &buffer, const DWORD receivedBytes,
                             const sockaddr_in &srcIp, 
                             const sockaddr_in &dstIp,
                             const SOCKET &acceptSocket) {

  TCPArrivedNetworkPackage packet;
  packet.buffer.clear();
  packet.buffer.insert(packet.buffer.end(), buffer.buf, buffer.buf + receivedBytes);
  packet.SetSrc(srcIp);

  packet.SetDst(dstIp);

  packet.rxTimeSec = std::chrono::system_clock::now();

  packet.socket = acceptSocket;

  _inputqueue.Push(std::move(packet));
}