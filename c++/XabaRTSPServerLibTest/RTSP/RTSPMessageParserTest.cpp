#include <pch.h>
#include <RTSP\RTSPMessageParser.h>
TEST(RTSPMesssageParserTest, OPTIONSMessage) {
	std::string message("OPTIONS rtsp://10.0.0.6:554 RTSP/1.0\r\nCSeq: 2\r\nUser-Agent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)\r\n\r\n");

	RTSPMessageParser parser;
	auto parseResult = parser.Parse(message);
	auto type = dynamic_cast<RTSPMessageOPTIONS*>(parseResult.get());

	EXPECT_TRUE(type != nullptr);

	EXPECT_EQ(type->CSeq(), 2);
	EXPECT_EQ(type->URL(), "rtsp://10.0.0.6:554"s);
	EXPECT_EQ(type->Version(), "RTSP/1.0"s);


}