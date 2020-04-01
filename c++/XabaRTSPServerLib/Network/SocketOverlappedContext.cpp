#include "pch.h"
#include "SocketOverlappedContext.h"

#define BUF_SIZE 2 * 1024 // Max RTP 1500 bytes

SocketOverlappedContext::SocketOverlappedContext() : SocketOverlappedContext(NULL) {}
SocketOverlappedContext::SocketOverlappedContext(SOCKET socket) : Socket(socket) {
  hEvent = 0;
  Buffer.buf = nullptr;
}

void SocketOverlappedContext::ResetBuffer() {
  if (Buffer.buf != nullptr) {
    delete[] Buffer.buf;
    Buffer.len = 0;
  }

  Buffer.len = BUF_SIZE;
  Buffer.buf = new char[BUF_SIZE];

  memset(&SrcIp, 0, sizeof(sockaddr_in));
}

SocketOverlappedContext::~SocketOverlappedContext() {
  delete[] Buffer.buf;
  Buffer.buf = nullptr;
}
