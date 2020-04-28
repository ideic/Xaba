#pragma once
#include <atomic>
#include <mutex>
#include <queue>
#include <optional>
#include <exception>
template <typename VALUE_TYPE>
class BlockingQueue {
    std::queue<VALUE_TYPE> _queue;
    mutable std::condition_variable _cv;
    mutable std::mutex _mtx;
    std::atomic_bool _terminated {false};

public:
    void Push(VALUE_TYPE&& value);
    VALUE_TYPE Pop();

    void Terminate();

    void Reset();

    bool Empty();
};

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::Push(VALUE_TYPE&& value)
{
    if (_terminated) {
        throw std::logic_error("BlockingQueue<VALUE_TYPE>::push(VALUE_TYPE&& value) was called in <terminated> state!");
    }
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _queue.push(std::move(value));
    }
    _cv.notify_one();
}

template <typename VALUE_TYPE>
VALUE_TYPE BlockingQueue<VALUE_TYPE>::Pop()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _cv.wait(lock, [this] { return (!_queue.empty() || _terminated); });

    if (_terminated)
    {
        throw std::logic_error("Queue terminated");
    }

    VALUE_TYPE result = std::move(_queue.front());
    _queue.pop();
    return result;
}

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::Terminate()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _terminated = true;
    _cv.notify_all();
}

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::Reset()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _terminated = false;
    std::queue<VALUE_TYPE> empty;
    std::swap(_queue, empty);
    _cv.notify_all();
}

template <typename VALUE_TYPE>
bool BlockingQueue<VALUE_TYPE>::Empty()
{
    return _queue.empty();
}
