#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <WS2tcpip.h>
#include "NetworkUtility.h"
class TCPArrivedNetworkPackage {
private:
    sockaddr_in _src{};
    std::string _srcIp;
    uint16_t _srcPort{};

    sockaddr_in _dst{};
    std::string _dstIp;
    uint16_t _dstPort{};

public:
        std::vector<char> buffer;
        std::chrono::time_point<std::chrono::system_clock> rxTimeSec;
        SOCKET socket{};
        void SetSrc(const sockaddr_in &sourceIp) {
            _src = sourceIp;
            auto [ip, port] = NetworkUtility::FromInAddrToStringIpPort(sourceIp);
            
            _srcIp = std::move(ip); 
            _srcPort = std::move(port);
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

        void SetDst(const sockaddr_in& dstip) {
            _dst = dstip;
            auto [ip, port] = NetworkUtility::FromInAddrToStringIpPort(dstip);

            _dstIp = std::move(ip);
            _dstPort = std::move(port);
        }
        const sockaddr_in& GetDst() const {
            return _dst;;
        }


        const std::string& DstIp() {
            return _dstIp;
        }

        const uint16_t& DstPort() {
            return _dstPort;
        }
};

class TCPResponseNetworkPackage : public TCPArrivedNetworkPackage {
};