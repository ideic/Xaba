#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <stdint.h>
#include <string>
#include <vector>

enum class OIOMode {
    NotSet,
    Accept,
    Read,
    Write,
    Send
};
class SocketOverlappedContext : public OVERLAPPED {
 private:
 public:
  SocketOverlappedContext() ;
  SocketOverlappedContext(SOCKET acceptSocket, SOCKET listenSocket);

  ~SocketOverlappedContext();

  void ResetBuffer();

  WSABUF Buffer{0};
  SOCKET AcceptSocket{INVALID_SOCKET};
  SOCKET ListenSocket{ INVALID_SOCKET };

  DWORD ReceivedBytes{0};
  DWORD Flags{0};
  sockaddr_in SrcIp{0};
  //int16_t SrcPort{0};
  int SrcIpLength { 0 };
  HANDLE ListenIOPort{0};
  HANDLE AcceptIOPort{ 0 };

  sockaddr_in DstIp{0};

  OIOMode State;

};

