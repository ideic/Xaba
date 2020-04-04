#include "pch.h"
#include <BlockingQueue.h>
#include <future>
#include <thread>
#include <chrono>
#include <Network/NetworkPackage.h>
using namespace std::chrono_literals;
TEST(BlockingQueueTest, PushAndPopWorks) {
    BlockingQueue<int> q;
    q.push(5);
    auto result = q.pop();
    EXPECT_EQ(result, 5);
}

TEST(BlockingQueueTest, PopBlocks) {
    BlockingQueue<int> q;

    auto fut = std::async(std::launch::async, [&q]() { 
        std::this_thread::sleep_for(200ms);
        q.push(5); 
    });
    auto now = std::chrono::system_clock::now();
    q.pop();
    auto now2 = std::chrono::system_clock::now();
    fut.get();
    EXPECT_GT(std::chrono::duration_cast<std::chrono::milliseconds>(now2 - now).count(), 150);
}

TEST(BlockingQueueTest, LoadTest) {
   
    BlockingQueue<NetworkPackage> q;
    std::vector<std::thread> writers(4);
    std::vector<std::thread> readers(4);
    std::atomic_int64_t sum = 0;
    std::atomic_int reads = 1000'000;

    for (int i = 0; i < 4; i++) {
        writers[i] = std::thread([i,&q]() {
            for (int j = 1; j <= 250'000; j++) {
                NetworkPackage obj;
                obj.buffer = std::vector<uint8_t>(1500);
                obj.dstIp = "192.168.0.1";
                obj.dstPort = 80;
                obj.rxTimeSec = std::chrono::system_clock::now();
                obj.dstIp = "172.75.01.08";
                obj.dstPort = 65535;
                int64_t id = i * (int64_t)250'000 + j;
                std::copy_n((uint8_t*)&id, sizeof(id), obj.buffer.data());

                q.push(obj);
            }
        });

        readers[i] = std::thread([&q, &sum, &reads]() {
            while (reads > 0) {
                auto obj = q.pop();
                if (reads < 1) continue;
                reads--;
                int64_t id { 0 };
                std::copy_n(obj.buffer.data(), sizeof(id), (uint8_t*) &id );
                sum += id;
            }
        });
    }
    for (int i = 0; i < 4; i++) {
        writers[i].join();
    }
    while (reads > 0) {
        std::this_thread::sleep_for(100ms);
    }
    q.terminate();
    for (int i = 0; i < 4; i++) {
        readers[i].join();
    }
    int64_t expected = ((int64_t)1'000'000 / 2)*(int64_t)(2*1 + (1'000'000 - 1) *1); // (n / 2)[2a + (n - 1) d]; // n = 1000 000 , a = first item , d = difference
    EXPECT_EQ(sum, expected);
}