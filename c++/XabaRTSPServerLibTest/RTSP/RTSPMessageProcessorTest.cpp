#include <pch.h>
#include <MessageProcessors/MessageProcessorRTSPMessage.h>
#include <Singleton.h>
#include <BlockingQueue.h>
TEST(RTSPMessageProcessorTest, OptionResponse) {
	MessageProcessorRTSPMessage msgProcessor;

	TCPArrivedNetworkPackage pckg;
    std::string msg("OPTIONS rtsp ://10.0.0.6:554 RTSP/1.0\r\nCSeq: 2\r\nUser-Agent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)\r\n\r\n");
    pckg.buffer = std::vector<char>(msg.begin(), msg.end());
    pckg.SetSrc(NetworkUtility::FromStringIpPortToInAddr("192.168.0.1", 80));
    pckg.rxTimeSec = std::chrono::system_clock::now();
    pckg.SetDst((NetworkUtility::FromStringIpPortToInAddr("172.75.01.08", 65535)));

	msgProcessor.ProcessNetworkPackage(pckg);

    auto& outputqueue = Singleton<BlockingQueue<TCPResponseNetworkPackage>>();

    auto response = outputqueue.Pop();

    std::string responseMsg(begin(response.buffer), end(response.buffer));

    EXPECT_TRUE("RTSP/1.0 200 OK\r\nCSeq:0\r\nPublic: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n"s == responseMsg);

    EXPECT_EQ(pckg.SrcIp(), response.DstIp());
    EXPECT_EQ(pckg.SrcPort(), response.DstPort());

    EXPECT_EQ(pckg.DstIp(), response.SrcIp());
    EXPECT_EQ(pckg.DstPort(), response.SrcPort());

}