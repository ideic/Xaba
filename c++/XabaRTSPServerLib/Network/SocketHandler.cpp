#include "pch.h"
#include "SocketHandler.h"
#include "ws2tcpip.h"
#include <stdexcept>
#include <mswsock.h>

void SocketHandler::CreateListenSocket(std::string_view host, int16_t portNumber, HANDLE &completionPort) {
  struct addrinfo hints, *addrInfoInit;
  ZeroMemory(&hints, sizeof(hints));

  // hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  int iResult = getaddrinfo(host.data(), std::to_string(portNumber).c_str(), &hints, &addrInfoInit);
  if (iResult != 0) {
    throw std::runtime_error("getaddrinfo failed with error:" + std::to_string(iResult));
  }

  std::shared_ptr<addrinfo> addrInfo(addrInfoInit, freeaddrinfo);

  auto listenSocket =  WSASocket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (listenSocket == INVALID_SOCKET) {
    throw std::runtime_error("socket failed with error: " + WSAGetLastError());
  }

  char opt = 1;
  if (setsockopt(listenSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, &opt,  sizeof(opt))) {
    iResult = WSAGetLastError();
    throw std::runtime_error( "Cannot set socket to use exclusive address. ErrorCode:" + iResult);
  }

  opt = 0;
  if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    iResult = WSAGetLastError();
    throw std::runtime_error("Cannot set socket to don't reuse address. ErrorCode:" + iResult);
  }

  if (bind(listenSocket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen) == SOCKET_ERROR) {
    iResult = WSAGetLastError();
    throw std::runtime_error("Cannot bind socket to local address. Error:" + iResult);
  }

  iResult = listen(listenSocket, SOMAXCONN);
  if (iResult != 0) {
      iResult = WSAGetLastError();
      throw std::runtime_error("Cannot listen on socket error:" + iResult);
  }

  completionPort = CreateIoCompletionPort((HANDLE)listenSocket, completionPort, portNumber, 0);

  if (completionPort == 0) {
      iResult = WSAGetLastError();
      closesocket(listenSocket);
      throw std::runtime_error("Cannot create listenport. ErrorCode:" + iResult);
  }

  Ctx = std::shared_ptr<SocketOverlappedContext>(new SocketOverlappedContext(), [](SocketOverlappedContext* ctx) {
      closesocket(ctx->ListenSocket);
      delete ctx;
      });
  
  Ctx->ListenSocket = listenSocket;
  Ctx->ListenIOPort = completionPort;
  Ctx->DstPort = portNumber;
  Ctx->DstIp = inet_ntoa(((sockaddr_in*)addrInfo->ai_addr)->sin_addr);
  Ctx->ResetBuffer();
  

  /*wchar_t ip[INET_ADDRSTRLEN];
  InetNtop(addrInfo->ai_family, &addrInfo->ai_addr, ip, INET_ADDRSTRLEN);
  */
}

void SocketHandler::CreateAcceptSocket(SocketOverlappedContext* pCtx, HANDLE& completionPort) {
    struct addrinfo hints, * addrInfoInit;
    ZeroMemory(&hints, sizeof(hints));

    // hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(pCtx->DstIp.c_str(), std::to_string(pCtx->DstPort).c_str(), &hints, &addrInfoInit);
    if (iResult != 0) {
        throw std::runtime_error("getaddrinfo failed with error:" + std::to_string(iResult));
    }

    std::shared_ptr<addrinfo> addrInfo(addrInfoInit, freeaddrinfo);


    Ctx = std::shared_ptr<SocketOverlappedContext>(new SocketOverlappedContext(), [](SocketOverlappedContext* ctx) {
        closesocket(ctx->AcceptSocket);
        delete ctx;
        });

    Ctx->ListenSocket = pCtx->ListenSocket;
    Ctx->ListenIOPort = pCtx->ListenIOPort;
    Ctx->DstPort = pCtx->DstPort;
    Ctx->DstIp = pCtx->DstIp;
    Ctx->ResetBuffer();

    Ctx->AcceptSocket = WSASocket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (Ctx->AcceptSocket == INVALID_SOCKET) {
        throw std::runtime_error("Accept socket failed with error: " + WSAGetLastError());
    }

    Ctx->State = OIOMode::Accept;
    iResult = AcceptEx(Ctx->ListenSocket, Ctx->AcceptSocket, Ctx->Buffer.buf/*Ctx->AcceptBuffer.data()*/,
        Ctx->Buffer.len - ((sizeof(sockaddr_in) + 16) * 2),
        sizeof(SOCKADDR_IN) + 16,
        sizeof(SOCKADDR_IN) + 16,
        &Ctx->ReceivedBytes, Ctx.get());
    if (iResult != 0) {
        iResult = WSAGetLastError();
        throw std::runtime_error("Cannot accept socket. Error:" + iResult);
    }

    completionPort = CreateIoCompletionPort((HANDLE)Ctx->AcceptSocket, completionPort, Ctx->DstPort, 0);

    if (completionPort == 0) {
        iResult = WSAGetLastError();
        closesocket(Ctx->AcceptSocket);
        throw std::runtime_error("Cannot create listenport. ErrorCode:" + iResult);
    }

    Ctx->AcceptIOPort = completionPort;
}