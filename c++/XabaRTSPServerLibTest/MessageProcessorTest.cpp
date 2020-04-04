#include "pch.h"
#include <BlockingQueue.h>
#include <Network/NetworkPackage.h>
#include <Singleton.h>
#include <MessageProcessors/MessageProcessor.h>
#include <chrono>
using namespace std::chrono_literals;

TEST(MessageProcessorTest, CallsSubscribedMethods) {
    BlockingQueue<NetworkPackage>& queue = Singleton<BlockingQueue<NetworkPackage>>();
    queue.Reset();
    MessageProcessor mp;
    NetworkPackage givenPckg {};
    mp.Subscribe([&givenPckg](const NetworkPackage& pckg) {
        givenPckg = pckg;
    });

    mp.Start(1);

    NetworkPackage newPackage;
    newPackage.buffer = std::vector<uint8_t> { 1, 2, 3, 4 };
    newPackage.dstIp = "127.0.0.1";
    newPackage.dstPort = 80;
    newPackage.rxTimeSec = std::chrono::system_clock::now();
    newPackage.srcIp = "192.168.0.1";
    newPackage.srcPort = 100;

    queue.Push(newPackage);

    uint8_t tries = 0;
    while (tries < 10 && givenPckg.buffer.empty()) {
        std::this_thread::sleep_for(10ms);
        tries++;
    }

    mp.Stop();

    EXPECT_TRUE( std::equal(newPackage.buffer.begin(), newPackage.buffer.end(), givenPckg.buffer.begin()));
    EXPECT_EQ(newPackage.dstIp, givenPckg.dstIp);
    EXPECT_EQ(newPackage.dstPort, givenPckg.dstPort);
    EXPECT_EQ(newPackage.rxTimeSec, givenPckg.rxTimeSec);
    EXPECT_EQ(newPackage.srcIp, givenPckg.srcIp);
    EXPECT_EQ(newPackage.srcPort, givenPckg.srcPort);
}

TEST(MessageProcessorTest, CallsAllSubscribedMethods)
{
    BlockingQueue<NetworkPackage>& queue = Singleton<BlockingQueue<NetworkPackage>>();
    queue.Reset();
    MessageProcessor mp;
    NetworkPackage givenPckg {};
    NetworkPackage givenPckg2 {};
    mp.Subscribe([&givenPckg](const NetworkPackage& pckg) {
        givenPckg = pckg;
    });

    mp.Subscribe([&givenPckg2](const NetworkPackage& pckg) {
        givenPckg2 = pckg;
    });

    mp.Start(1);

    NetworkPackage newPackage;
    newPackage.buffer = std::vector<uint8_t> { 1, 2, 3, 4 };
    newPackage.dstIp = "127.0.0.1";
    newPackage.dstPort = 80;
    newPackage.rxTimeSec = std::chrono::system_clock::now();
    newPackage.srcIp = "192.168.0.1";
    newPackage.srcPort = 100;

    queue.Push(newPackage);

    uint8_t tries = 0;
    while (tries < 10 && (givenPckg.buffer.empty() || givenPckg2.buffer.empty())) {
        std::this_thread::sleep_for(10ms);
        tries++;
    }

    mp.Stop();

    EXPECT_TRUE(std::equal(newPackage.buffer.begin(), newPackage.buffer.end(), givenPckg.buffer.begin()));
    EXPECT_EQ(newPackage.dstIp, givenPckg.dstIp);
    EXPECT_EQ(newPackage.dstPort, givenPckg.dstPort);
    EXPECT_EQ(newPackage.rxTimeSec, givenPckg.rxTimeSec);
    EXPECT_EQ(newPackage.srcIp, givenPckg.srcIp);
    EXPECT_EQ(newPackage.srcPort, givenPckg.srcPort);

    EXPECT_TRUE(std::equal(newPackage.buffer.begin(), newPackage.buffer.end(), givenPckg2.buffer.begin()));
    EXPECT_EQ(newPackage.dstIp, givenPckg2.dstIp);
    EXPECT_EQ(newPackage.dstPort, givenPckg2.dstPort);
    EXPECT_EQ(newPackage.rxTimeSec, givenPckg2.rxTimeSec);
    EXPECT_EQ(newPackage.srcIp, givenPckg2.srcIp);
    EXPECT_EQ(newPackage.srcPort, givenPckg2.srcPort);
}