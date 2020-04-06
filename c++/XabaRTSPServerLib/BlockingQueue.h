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
    void Push(VALUE_TYPE&& value);
    VALUE_TYPE Pop();

    void Terminate();

    void Reset();
};

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::Push(VALUE_TYPE&& value)
{
    if (terminated) {
        throw std::logic_error("BlockingQueue<VALUE_TYPE>::push(VALUE_TYPE&& value) was called in <terminated> state!");
    }
    {
        std::unique_lock<std::mutex> lock(mtx);
        items.push(std::move(value));
    }
    cv.notify_one();
}

template <typename VALUE_TYPE>
VALUE_TYPE BlockingQueue<VALUE_TYPE>::Pop()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return (!items.empty() || terminated); });

    if (terminated) return VALUE_TYPE {};

    VALUE_TYPE result = std::move(items.front());
    items.pop();
    return result;
}

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::Terminate()
{
    std::unique_lock<std::mutex> lock(mtx);
    terminated = true;
    cv.notify_all();
}

template <typename VALUE_TYPE>
void BlockingQueue<VALUE_TYPE>::Reset()
{
    std::unique_lock<std::mutex> lock(mtx);
    terminated = false;
    cv.notify_all();
}
