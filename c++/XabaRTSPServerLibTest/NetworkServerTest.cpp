#include "pch.h"
#include <Network/NetworkServer.h>
#include "NetworkTestClient.h"
#include <Singleton.h>
#include <thread>
#include <Logger\LoggerFactory.h>
#include <algorithm>
#include <execution>

using namespace std::chrono_literals;
using namespace std::string_literals;

TEST(NetworkServerTest, ServerGetSentPackage) {
	auto& queue = Singleton<BlockingQueue<NetworkPackage>>();
	queue.Reset();

	NetworkServer server;
	server.StartServer("127.0.0.10"s, { 554 }, 1);

	NetworkTestClient client;

	client.Init("127.0.0.10"s, "554"s);

	std::string testMessage = "TEST MESSAGE"s;
	client.Send(testMessage);

	auto package = queue.Pop();

	std::vector<uint8_t> expectedMessage(begin(testMessage), end(testMessage));
	EXPECT_TRUE(std::equal(begin(package.buffer), end(package.buffer), begin(expectedMessage)));

	server.StopServer();

	while (!server.Stopped()) {
		std::this_thread::sleep_for(100ms);
	}
}

TEST(NetworkServerTest, ServerGetMultipleSentPackage) {
	auto& queue = Singleton<BlockingQueue<NetworkPackage>>();
	queue.Reset();

	NetworkServer server;
	server.StartServer("127.0.0.10"s, { 554 }, 1);

	NetworkTestClient client;

	client.Init("127.0.0.10"s, "554"s);

	std::string testMessage = "TEST MESSAGE"s;
	client.Send(testMessage);
	std::string testMessage2 = "TEST MESSAGE2"s;
	client.Send(testMessage2);

	auto package = queue.Pop();
	std::vector<uint8_t> expectedMessage(begin(testMessage), end(testMessage));
	EXPECT_TRUE(std::equal(begin(package.buffer), end(package.buffer), begin(expectedMessage)));

	package = queue.Pop();
	expectedMessage = std::vector<uint8_t> (begin(testMessage2), end(testMessage2));
	EXPECT_TRUE(std::equal(begin(package.buffer), end(package.buffer), begin(expectedMessage)));


	server.StopServer();

	while (!server.Stopped()) {
		std::this_thread::sleep_for(100ms);
	}
}

TEST(NetworkServerTest, ServerGetMultipleSentPackageLoadTest) {
	auto& queue = Singleton<BlockingQueue<NetworkPackage>>();
	queue.Reset();
	NetworkServer server;
	server.StartServer("127.0.0.10"s, { 554 }, 2);

	NetworkTestClient client;

	client.Init("127.0.0.10"s, "554"s);

	for (int i = 100'000; i <= 200'000; i++) {
		std::string testMessage = "TM_"s + std::to_string(i);
		client.Send(testMessage);
	}

	std::vector<std::string> givenMessages;
	bool lastMessageArrived = false;
	while (!lastMessageArrived) {
		auto package = queue.Pop();
		auto message = std::string(begin(package.buffer), end(package.buffer));
		givenMessages.push_back(message);

		if (message.find("TM_200000") != std::string::npos) {
			lastMessageArrived = true;
		}

	}

	server.StopServer();

	while (!server.Stopped()) {
		std::this_thread::sleep_for(100ms);
	}

	int missingMesages = 0;
	for (int i = 100'000; i <= 200'000; i++) {
		auto found = std::find_if(std::execution::par,
			begin(givenMessages),
			end(givenMessages),
			[&i](const std::string& msg) {
				return msg.find("TM_"s + std::to_string(i)) != std::string::npos;
			});

		//EXPECT_TRUE(found != end(givenMessages));
		if (found == end(givenMessages)) {
			missingMesages++;
		}
		if (found != end(givenMessages) && found->length() == ("TM_"s + std::to_string(i)).length()) {
			givenMessages.erase(found);
		}
	}
	LOGGER->LogInfo("Missing message count:"s + std::to_string(missingMesages));
	EXPECT_LT((double)missingMesages, 100'000 *0.01);
}


TEST(NetworkServerTest, ServerGetMultiplePackageFromMultipleClient) {
	auto& queue = Singleton<BlockingQueue<NetworkPackage>>();
	queue.Reset();
	NetworkServer server;
	server.StartServer("127.0.0.10"s, { 554 }, 2);

	NetworkTestClient client;
	client.Init("127.0.0.10"s, "554"s);

	NetworkTestClient client2;
	client2.Init("127.0.0.10"s, "554"s);

	std::thread t1([&client]() {
		for (int i = 100'000; i <= 200'000; i++) {
			std::string testMessage = "TM_1_"s + std::to_string(i);
			client.Send(testMessage);
		}
	});

	std::thread t2([&client2]() {
		for (int i = 100'000; i <= 200'000; i++) {
			std::string testMessage = "TM_2_"s + std::to_string(i);
			client2.Send(testMessage);
		}
	});

	
	t1.join();
	t2.join();
	LOGGER->LogInfo("Sending finished");
	client.Send("LAST_MESSAGE");
	client2.Send("LAST_MESSAGE");
	std::vector<std::string> givenMessages;
	bool lastMessageArrived = false;
	while (!lastMessageArrived) {
		auto package = queue.Pop();
		auto message = std::string(begin(package.buffer), end(package.buffer));
		givenMessages.push_back(message);

		if (message.find("LAST_MESSAGE") != std::string::npos) {
			lastMessageArrived = true;
		}

	}
	LOGGER->LogInfo("Last Message Arrived");
	server.StopServer();

	while (!server.Stopped()) {
		std::this_thread::sleep_for(100ms);
	}

	int missingMesages = 0;
	for (int i = 100'000; i <= 200'000; i++) {
		auto found = std::find_if(std::execution::par,
			begin(givenMessages),
			end(givenMessages),
			[&i](const std::string& msg) {
				return msg.find("TM_1_"s + std::to_string(i)) != std::string::npos;
			});

		//EXPECT_TRUE(found != end(givenMessages));
		if (found == end(givenMessages)) {
			missingMesages++;
		}
		if (found != end(givenMessages) && found->length() == ("TM_1_"s + std::to_string(i)).length()) {
			givenMessages.erase(found);
		}

		auto found2 = std::find_if(std::execution::par,
			begin(givenMessages),
			end(givenMessages),
			[&i](const std::string& msg) {
				return msg.find("TM_2_"s + std::to_string(i)) != std::string::npos;
			});

		//EXPECT_TRUE(found != end(givenMessages));
		if (found2 == end(givenMessages)) {
			missingMesages++;
		}
		if (found2 != end(givenMessages) && found2->length() == ("TM_2_"s + std::to_string(i)).length()) {
			givenMessages.erase(found2);
		}
	}
	LOGGER->LogInfo("Missing message count:"s + std::to_string(missingMesages));
	EXPECT_LT(double(missingMesages), 200'000*0.01);
}

TEST(NetworkServerTest, ServerGetMultiplePackageFromMultipleClientOneSend) {
	auto& queue = Singleton<BlockingQueue<NetworkPackage>>();
	queue.Reset();
	NetworkServer server;
	server.StartServer("127.0.0.10"s, { 554 }, 1);

	NetworkTestClient client;
	client.Init("127.0.0.10"s, "554"s);

	NetworkTestClient client2;
	client2.Init("127.0.0.10"s, "554"s);

	std::thread t1([&client]() {
			std::string testMessage = "TM_1_100000"s;
			client.Send(testMessage);
		});

	std::thread t2([&client2]() {
			std::string testMessage = "TM_2_100000"s;
			client2.Send(testMessage);
		});

	t1.join();
	t2.join();


	LOGGER->LogInfo("Sending finished");
	std::vector<std::string> givenMessages;

	while (givenMessages.size() < 2 || (givenMessages.size() == 1 && givenMessages.front().size() < 12)) {
		auto package = queue.Pop();
		auto message = std::string(begin(package.buffer), end(package.buffer));
		givenMessages.push_back(message);
	}
	LOGGER->LogInfo("Last Message Arrived");
	server.StopServer();

	while (!server.Stopped()) {
		std::this_thread::sleep_for(100ms);
	}
	LOGGER->LogInfo("givenmessage size:"s + std::to_string(givenMessages.size()));

	for (auto& msg : givenMessages) {
		LOGGER->LogInfo("Message:"s + msg);
	}

	auto found = std::find_if(
		begin(givenMessages),
		end(givenMessages),
		[](const std::string& msg) {
			return msg.find("TM_1_100000"s) != std::string::npos;
		});

	EXPECT_TRUE(found != end(givenMessages));
	
	found = std::find_if(
		begin(givenMessages),
		end(givenMessages),
		[](const std::string& msg) {
			return msg.find("TM_2_100000"s) != std::string::npos;
		});

	EXPECT_TRUE(found != end(givenMessages));
}