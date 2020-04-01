#pragma once
#include <string>
#include <vector>
#include <chrono>
struct NetworkPackage {
        std::vector<uint8_t> buffer;
        std::string srcIp;
        uint16_t srcPort;
        std::string dstIp;
        uint16_t dstPort;
        std::chrono::time_point<std::chrono::system_clock> rxTimeSec;
};
