#include "pch.h"
#include <string>     
#include <iostream> 
#include <sstream>
#include <Logger\ConsoleLogger.h>
#include <thread>
using namespace std::chrono_literals;

TEST(ConsoleLoggerTest, WarningLogsExpectedMessage) {

	std::stringstream out;
	std::streambuf* oldbuf = std::cout.rdbuf(out.rdbuf()); //save old buf
	ConsoleLogger logger;
	logger.LogWarning("Test");
	int i = 0;
	while (i < 10 && out.tellp() == 0) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	std::cout.rdbuf(oldbuf); //reset to standard output again
	EXPECT_TRUE(out.str().find("WARNING", 0) != std::string::npos);
	EXPECT_TRUE(out.str().find("Test", 0) != std::string::npos);
}

TEST(ConsoleLoggerTest, InfoLogsExpectedMessage) {

	std::stringstream out;
	std::streambuf* oldbuf = std::cout.rdbuf(out.rdbuf()); //save old buf
	ConsoleLogger logger;
	logger.LogInfo("TestInfo");
	int i = 0;
	while (i < 10 && out.tellp() == 0) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	std::cout.rdbuf(oldbuf); //reset to standard output again
	EXPECT_TRUE(out.str().find("INFO", 0) != std::string::npos);
	EXPECT_TRUE(out.str().find("TestInfo", 0) != std::string::npos);
}

TEST(ConsoleLoggerTest, ErrorLogsExpectedMessage) {

	std::stringstream out;
	std::streambuf* oldbuf = std::cerr.rdbuf(out.rdbuf()); //save old buf
	ConsoleLogger logger;
	logger.LogError(std::exception("TestError"), "ErrorMessage");
	int i = 0; 
	while (i < 10 && out.tellp() == 0) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	std::cerr.rdbuf(oldbuf); //reset to standard output again
	EXPECT_TRUE(out.str().find("ERROR", 0) != std::string::npos);
	EXPECT_TRUE(out.str().find("ErrorMessage", 0) != std::string::npos);
	EXPECT_TRUE(out.str().find("TestError", 0) != std::string::npos);
}

TEST(ConsoleLoggerTest, ErrorAlsoLogsOnCout) {

	std::stringstream out;
	std::streambuf* oldbuf = std::cout.rdbuf(out.rdbuf()); //save old buf
	ConsoleLogger logger;
	logger.LogError(std::exception("TestError"), "ErrorMessage");
	int i = 0;
	while (i < 10 && out.tellp() == 0) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	std::cout.rdbuf(oldbuf); //reset to standard output again
	EXPECT_TRUE(out.str().find("ERROR", 0) != std::string::npos);
	EXPECT_TRUE(out.str().find("ErrorMessage", 0) != std::string::npos);
	EXPECT_TRUE(out.str().find("TestError", 0) != std::string::npos);
}