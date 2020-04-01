#pragma once
#include <memory>
#include <string>
#include <WinSock2.h>
#include "SocketOverlappedContext.h"
class SocketHandler {
 public:
  void CreateSocket(std::string_view host, int16_t portNumber, HANDLE completionPort);

  std::shared_ptr<SocketOverlappedContext> Ctx;
};
