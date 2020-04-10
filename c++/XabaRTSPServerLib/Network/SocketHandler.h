#pragma once
#include <memory>
#include <string>
#include <WinSock2.h>
#include "SocketOverlappedContext.h"
class SocketHandler {
 public:
  void CreateListenSocket(std::string_view host, int16_t portNumber, HANDLE &completionPort);
  void CreateAcceptSocket(SocketOverlappedContext* Ctx, HANDLE& completionPort);
  std::shared_ptr<SocketOverlappedContext> Ctx;
};
