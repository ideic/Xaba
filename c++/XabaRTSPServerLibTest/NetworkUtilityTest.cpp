#include "pch.h"
#include <Network/NetworkUtility.h>
TEST(NetworkUtilityTest, FullTest) {
	auto ipaddr = NetworkUtility::FromStringIpPortToInAddr("192.168.10.1", 80);
	auto [ip, port] = NetworkUtility::FromInAddrToStringIpPort(ipaddr);

	EXPECT_EQ("192.168.10.1", ip);
	EXPECT_EQ(80, port);
}