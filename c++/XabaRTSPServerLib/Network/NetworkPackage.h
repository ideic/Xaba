#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <WS2tcpip.h>

class TCPArrivedNetworkPackage {
private:
    sockaddr_in _src;
    std::string _srcIp;
    uint16_t _srcPort;
public:
        std::vector<char> buffer;
        std::string dstIp;
        uint16_t dstPort;
        std::chrono::time_point<std::chrono::system_clock> rxTimeSec;
        void SetSrc(const sockaddr_in &sourceIp) {
            _src = sourceIp;
            char str[INET_ADDRSTRLEN];
            _srcIp = inet_ntop(AF_INET, &(sourceIp.sin_addr), str, INET_ADDRSTRLEN);
            _srcPort = ntohs(sourceIp.sin_port);
        }

        const sockaddr_in &GetSrc() const {
            return _src;;
        }


        const std::string& SrcIp() {
            return _srcIp;
        }

        const uint16_t &SrcPort() {
            return _srcPort;
        }
};

class TCPResponseNetworkPackage : public TCPArrivedNetworkPackage {
};