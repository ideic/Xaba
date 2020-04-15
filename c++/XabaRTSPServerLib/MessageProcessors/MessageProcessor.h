#pragma once
#include <BlockingQueue.h>
#include <Network\NetworkPackage.h>
#include <Singleton.h>
#include <list>
#include <functional>
class MessageProcessor {
private:
    BlockingQueue<TCPArrivedNetworkPackage>& _queue = Singleton<BlockingQueue<TCPArrivedNetworkPackage>>();
    std::vector<std::thread> _workers;
    std::atomic_bool _finished { false };
    std::atomic_int _subscriberId { 0 };
    std::list<std::tuple<int, std::function<void(const TCPArrivedNetworkPackage&)>>> _subscribers;
    void Worker();

public:
    void Start(uint8_t numberOfThreads);
    void Stop();
    int Subscribe(std::function<void(const TCPArrivedNetworkPackage&)> function);
    void UnSubscribe(int id);
    MessageProcessor() = default;
    ~MessageProcessor();
};
