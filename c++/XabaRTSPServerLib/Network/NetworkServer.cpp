#include "pch.h"
#include "NetworkServer.h"
#include <stdexcept>
#include <Logger\LoggerFactory.h>
#include <algorithm>
#include "SocketOverlappedContext.h"
#include "SocketHandler.h"
#include "NetworkPackage.h"

void NetworkServer::StartWorkers(uint8_t numberOfThreads) {
  for (int i = 0; i < numberOfThreads; ++i) {
    _workers.emplace_back([&]() { Worker(); });
  }
}

void NetworkServer::Worker() {
  while (!_finish) {

    DWORD numberOfBytes = 0;
    uint64_t completionKey = 0;
    LPOVERLAPPED ctx = 0;

    auto ioSucceeds = GetQueuedCompletionStatus(_completionPort, 
                                                &numberOfBytes,
                                                &completionKey, 
                                                &ctx,
                                                1000 //  dwMilliseconds
    );

    if (ioSucceeds) {
      if (&numberOfBytes > 0) {

        auto overlappedContext = (SocketOverlappedContext *)ctx;

        SaveData(overlappedContext->Buffer, numberOfBytes,
                 overlappedContext->SrcIp, 
                 overlappedContext->DstIp, overlappedContext->DstPort);

        overlappedContext->ResetBuffer();

        int iresult = WSARecvFrom(
            overlappedContext->Socket, &overlappedContext->Buffer, 1,
            &overlappedContext->ReceivedBytes, &overlappedContext->Flags,
            (sockaddr *)&overlappedContext->SrcIp,
            &overlappedContext->FromLength, overlappedContext, NULL);
       
        if (iresult != 0) {
          iresult = WSAGetLastError();
          if (iresult != WSA_IO_PENDING) {
            LOGGER->LogWarning("RecordingServer IOCP 2nd WSARecvFrom init failed with Code:" + iresult);
            // socket closed;
            iresult = 0;
          }
        }
      } else {
        LOGGER->LogWarning("RecordingServer Number of received bytes 0");
      }
    } else {
      auto iResult = WSAGetLastError();

      if (iResult != WAIT_TIMEOUT) {
        LOGGER->LogWarning("RecordingServer IOCP GetQueuedCompletionStatus failed with Code:" + iResult);
        // Init Receive ?
      }
    }
  }
}

void NetworkServer::StartServer(std::string_view host,
                                const std::vector<int> &ports,
                                uint8_t numberOfThreads) {
    _finish = false;

  _completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

  if (!_completionPort) {
    throw std::runtime_error("IO Completion port create failed with error: " +  GetLastError());
  }

  LOGGER->LogInfo("Start RecordingServer Worker threads:" + std::to_string(numberOfThreads));

  StartWorkers(numberOfThreads);

  LOGGER->LogInfo("Start Listening");

  std::for_each(ports.begin(), ports.end(),[this, &host](const int port) { CreatePort(host, port); });
}

void NetworkServer::CreatePort(std::string_view host, int port)
{

  std::shared_ptr<SocketHandler> socket = std::make_shared<SocketHandler>();

  try {
    socket->CreateSocket(host, port, _completionPort);

    _openPorts.push_back(socket);
  } catch (const std::exception &e) {
    LOGGER->LogError(e, "Create socker error on host:" + std::string(host) + "port:" + std::to_string(port));
    return;
  }

  auto &ctx = socket->Ctx;
  ctx->ResetBuffer();

  int iresult = WSARecvFrom(ctx->Socket, &ctx->Buffer, 1, &ctx->ReceivedBytes,
                            &ctx->Flags, (sockaddr *)&ctx->SrcIp,
                            &ctx->FromLength, ctx.get(), NULL);

  if (iresult != 0) {
    iresult = WSAGetLastError();
    if (iresult != WSA_IO_PENDING) {
      LOGGER->LogWarning("RecordingServer IOCP 1st WSARecvFrom init failed with Code:" + iresult);
      // socket closed;
      iresult = 0;
    }
  }
}

void NetworkServer::StopServer() {
  _finish = true;
  LOGGER->LogInfo("RecordingServer Stop Workers");

  for (auto &worker : _workers) {
    worker.join();
  };

  LOGGER->LogInfo("RecordingServer Clear OpenPorts");
  _openPorts.clear();

  LOGGER->LogInfo("RecordingServer Clear Workers");
  _workers.clear();
}


void NetworkServer::SaveData(const WSABUF &buffer, const DWORD receivedBytes,
                             const sockaddr_in &srcIp, 
                             const std::string &dstIp, int16_t dstPort) {
  // wchar_t ip[INET_ADDRSTRLEN];
  // InetNtop(from.sin_family, &from.sin_addr, ip, INET_ADDRSTRLEN);

  NetworkPackage packet;
  packet.buffer.clear();
  packet.buffer.insert(packet.buffer.end(), buffer.buf, buffer.buf + receivedBytes);
  packet.srcIp = inet_ntoa(((sockaddr_in)srcIp).sin_addr);
  packet.srcPort = ntohs(srcIp.sin_port);

  packet.dstIp = dstIp;
  packet.dstPort = dstPort;

  packet.rxTimeSec = std::chrono::system_clock::now();

  _queue.push(packet);
}