#include "pch.h"
#include "NetworkUtility.h"
#include <WS2tcpip.h>

std::tuple<std::string, uint16_t> NetworkUtility::FromInAddrToStringIpPort(const sockaddr_in& ip){
    char str[INET_ADDRSTRLEN];
    return std::make_tuple<std::string, std::uint16_t>(
        std::string(inet_ntop(AF_INET, &(ip.sin_addr), str, INET_ADDRSTRLEN)),
        ntohs(ip.sin_port));
}

sockaddr_in NetworkUtility::FromStringIpPortToInAddr(const std::string& ip, uint16_t port){
    
    struct sockaddr_in sa;
    inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    sa.sin_port = htons(port);

    return sa;
}
