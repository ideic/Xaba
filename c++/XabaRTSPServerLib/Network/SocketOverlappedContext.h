#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <stdint.h>
#include <string>

class SocketOverlappedContext : public OVERLAPPED {
 private:
 public:
  SocketOverlappedContext() ;
  SocketOverlappedContext(SOCKET socket);

  ~SocketOverlappedContext();

  void ResetBuffer();

  WSABUF Buffer{0};
  SOCKET Socket{0};
  DWORD ReceivedBytes{0};
  DWORD Flags{0};
  sockaddr_in SrcIp{0};
  //int16_t SrcPort{0};
  int FromLength { sizeof(sockaddr_in) };
  HANDLE IOPort{0};

  int16_t DstPort{0};
  std::string DstIp{0};
};

