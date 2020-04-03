#include "pch.h"
#include "MessageProcessor.h"
#include "Logger/LoggerFactory.h"
#include <future>
using namespace std::string_literals;
void MessageProcessor::Worker(){
   
    while (!_finished) {
        std::vector<std::future<void>> asyncResult; 
        auto pckg = _queue.pop();
        for(auto[id,  func] : _subscribers){
            asyncResult.push_back( std::async(std::launch::async, func, pckg));
        }

        for (auto& futResult : asyncResult) { /// other thread gets the next package
            try {
                futResult.get();
            } 
            catch (const std::exception& exc) {
                LOGGER->LogError(exc, "Package processing failed:"s + exc.what());
            }
        }
    }
}

void MessageProcessor::Start(uint8_t numberOfThreads)
{
    for (int i = 0; i < numberOfThreads; ++i) {
        _workers.emplace_back([&]() { Worker(); });
    }
}

void MessageProcessor::Stop(){
    _finished = true;
    LOGGER->LogInfo("MessageProcessor Stops Workers");

    for (auto& worker : _workers) {
        worker.join();
    };
}

int MessageProcessor::Subscribe(std::function<void(const NetworkPackage&)> function){
    auto id = ++_subscriberId;
    _subscribers.emplace_back(id, function);
    return id;
}

void MessageProcessor::UnSubscribe(int id)
{
    auto item = std::remove_if(_subscribers.begin(), _subscribers.end(), [id](std::tuple<int, std::function<void(const NetworkPackage&)>> item) {
        auto& [pid, pf] = item;
        return pid == id;
    });

}
