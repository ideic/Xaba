#pragma once
#include <BlockingQueue.h>
#include <Network\NetworkPackage.h>
#include <Singleton.h>
#include <list>
#include <functional>
class MessageProcessor {
private:
    BlockingQueue<NetworkPackage>& _queue = Singleton<BlockingQueue<NetworkPackage>>();
    std::vector<std::thread> _workers;
    std::atomic_bool _finished { false };
    std::atomic_int _subscriberId { 0 };
    std::list<std::tuple<int, std::function<void(const NetworkPackage&)>>> _subscribers;
    void Worker();

public:
    void Start(uint8_t numberOfThreads);
    void Stop();
    int Subscribe(std::function<void(const NetworkPackage&)> function);
    void UnSubscribe(int id);
};
