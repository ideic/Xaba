#pragma once
#include <atomic>
#include <mutex>
#include <queue>
template <typename VALUE_TYPE>
class BlockingQueue {
    std::queue<VALUE_TYPE> items;
    mutable std::condition_variable cv;
    mutable std::mutex mtx;
    std::atomic_bool terminated {false};

public:
    void push(VALUE_TYPE&& value);
    VALUE_TYPE getNext();

    void terminate();
};

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::push(VALUE_TYPE&& value)
{
    if (terminated) {
        throw std::logic_error("BlockingQueue<VALUE_TYPE>::push(VALUE_TYPE&& value) was called in <terminated> state!");
    }
    {
        std::unique_lock<std::mutex> lock(mtx);
        items.push(std::forward<VALUE_TYPE>(value));
    }
    cv.notify_one();
}

template <typename VALUE_TYPE>
VALUE_TYPE BlockingQueue<VALUE_TYPE>::getNext()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return (!items.empty() || terminated); });

    VALUE_TYPE result = std::move(items.front());
    items.pop();
    return result;
}

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::terminate()
{
    std::unique_lock<std::mutex> lock(mtx);
    terminated = true;
    cv.notify_all();
}
