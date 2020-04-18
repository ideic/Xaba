#include "pch.h"
#include <BlockingQueue.h>
#include <Network/NetworkPackage.h>
#include <Singleton.h>
#include <MessageProcessors/MessageProcessor.h>
#include <chrono>
#include <ws2tcpip.h>
using namespace std::chrono_literals;
using namespace std::string_literals;

TEST(MessageProcessorTest, CallsSubscribedMethods) {
    BlockingQueue<TCPArrivedNetworkPackage>& queue = Singleton<BlockingQueue<TCPArrivedNetworkPackage>>();
    queue.Reset();
    MessageProcessor mp;
    TCPArrivedNetworkPackage givenPckg {};
    mp.Subscribe([&givenPckg](const TCPArrivedNetworkPackage& pckg) {
        givenPckg = pckg;
    });

    mp.Start(1);
    auto rxTimeSec = std::chrono::system_clock::now();
   
    TCPArrivedNetworkPackage newPackage;
    newPackage.buffer = std::vector<char> { 1, 2, 3, 4 };
    newPackage.SetDst(NetworkUtility::FromStringIpPortToInAddr("127.0.0.1"s, 80));
    newPackage.rxTimeSec = rxTimeSec;
    newPackage.SetSrc(NetworkUtility::FromStringIpPortToInAddr("192.168.0.1"s, 100));
 
    queue.Push(std::move(newPackage));

    newPackage.buffer = std::vector<char> { 1, 2, 3, 4 };
    newPackage.SetDst(NetworkUtility::FromStringIpPortToInAddr("127.0.0.1"s, 80));
    newPackage.rxTimeSec = rxTimeSec;
    newPackage.SetSrc(NetworkUtility::FromStringIpPortToInAddr("192.168.0.1"s, 100));

    uint8_t tries = 0;
    while (tries < 10 && givenPckg.buffer.empty()) {
        std::this_thread::sleep_for(10ms);
        tries++;
    }

    mp.Stop();

    EXPECT_TRUE( std::equal(newPackage.buffer.begin(), newPackage.buffer.end(), givenPckg.buffer.begin()));
    EXPECT_EQ(newPackage.DstIp(), givenPckg.DstIp());
    EXPECT_EQ(newPackage.DstPort(), givenPckg.DstPort());
    EXPECT_EQ(newPackage.rxTimeSec, givenPckg.rxTimeSec);
    EXPECT_EQ(newPackage.SrcIp(), givenPckg.SrcIp());
    EXPECT_EQ(newPackage.SrcPort(), givenPckg.SrcPort());
}

TEST(MessageProcessorTest, CallsAllSubscribedMethods)
{
    BlockingQueue<TCPArrivedNetworkPackage>& queue = Singleton<BlockingQueue<TCPArrivedNetworkPackage>>();
    queue.Reset();
    MessageProcessor mp;
    TCPArrivedNetworkPackage givenPckg {};
    TCPArrivedNetworkPackage givenPckg2 {};
    mp.Subscribe([&givenPckg](const TCPArrivedNetworkPackage& pckg) {
        givenPckg = pckg;
    });

    mp.Subscribe([&givenPckg2](const TCPArrivedNetworkPackage& pckg) {
        givenPckg2 = pckg;
    });

    mp.Start(1);

    auto rxTimeSec = std::chrono::system_clock::now();
 
    TCPArrivedNetworkPackage newPackage;
    newPackage.buffer = std::vector<char> { 1, 2, 3, 4 };
    newPackage.SetDst(NetworkUtility::FromStringIpPortToInAddr("127.0.0.1"s, 80));
    newPackage.rxTimeSec = rxTimeSec;
    newPackage.SetSrc(NetworkUtility::FromStringIpPortToInAddr("192.168.0.1"s, 100));


    queue.Push(std::move(newPackage));

    newPackage.buffer = std::vector<char> { 1, 2, 3, 4 };
    newPackage.SetDst(NetworkUtility::FromStringIpPortToInAddr("127.0.0.1"s, 80));
    newPackage.rxTimeSec = rxTimeSec;
    newPackage.SetSrc(NetworkUtility::FromStringIpPortToInAddr("192.168.0.1"s, 100));

    uint8_t tries = 0;
    while (tries < 10 && (givenPckg.buffer.empty() || givenPckg2.buffer.empty())) {
        std::this_thread::sleep_for(10ms);
        tries++;
    }

    mp.Stop();

    EXPECT_TRUE(std::equal(newPackage.buffer.begin(), newPackage.buffer.end(), givenPckg.buffer.begin()));
    EXPECT_EQ(newPackage.DstIp(), givenPckg.DstIp());
    EXPECT_EQ(newPackage.DstPort(), givenPckg.DstPort());
    EXPECT_EQ(newPackage.rxTimeSec, givenPckg.rxTimeSec);
    EXPECT_EQ(newPackage.SrcIp(), givenPckg.SrcIp());
    EXPECT_EQ(newPackage.SrcPort(), givenPckg.SrcPort());

    EXPECT_TRUE(std::equal(newPackage.buffer.begin(), newPackage.buffer.end(), givenPckg2.buffer.begin()));
    EXPECT_EQ(newPackage.DstIp(), givenPckg2.DstIp());
    EXPECT_EQ(newPackage.DstPort(), givenPckg2.DstPort());
    EXPECT_EQ(newPackage.rxTimeSec, givenPckg2.rxTimeSec);
    EXPECT_EQ(newPackage.SrcIp(), givenPckg2.SrcIp());
    EXPECT_EQ(newPackage.SrcPort(), givenPckg2.SrcPort());
}