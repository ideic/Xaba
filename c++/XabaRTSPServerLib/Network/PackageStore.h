#pragma once
#include <map>
#include <string>
#include <Network\NetworkPackage.h>
#include <BlockingQueue.h>
class PackageStore {
private:
    std::map<std::string, BlockingQueue<NetworkPackage>> ip_port_queue;

public:
    void push(NetworkPackage package);
};