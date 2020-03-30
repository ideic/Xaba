#include "pch.h"
#include <Logger/AggregateLogger.h>
#include "TestLogger.h"
#include <thread>
using namespace std::string_literals;
using namespace std::chrono_literals;
TEST(AggreagateLoggetTest, LogWarningCallsLogWarningOnEachLogger) {
	auto logger1 = std::make_shared<TestLogger>();
	auto logger2 = std::make_shared<TestLogger>();
	AggregateLogger aLogger({ logger1, logger2 });
	aLogger.LogWarning("Test"s);
	int i = 0;
	while (i < 10 && !(logger1->IsWarningCalled())) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	EXPECT_TRUE(logger1->IsWarningCalled());
	EXPECT_TRUE(logger2->IsWarningCalled());
}

TEST(AggreagateLoggetTest, LogInfoCallsLogInfoOnEachLogger) {
	auto logger1 = std::make_shared<TestLogger>();
	auto logger2 = std::make_shared<TestLogger>();
	AggregateLogger aLogger({ logger1, logger2 });
	aLogger.LogInfo("Test"s);
	int i = 0;
	while (i < 10 && !(logger1->IsInfoCalled())) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	EXPECT_TRUE(logger1->IsInfoCalled());
	EXPECT_TRUE(logger2->IsInfoCalled());
}

TEST(AggreagateLoggetTest, LogErrorCallsLogErrorOnEachLogger) {
	auto logger1 = std::make_shared<TestLogger>();
	auto logger2 = std::make_shared<TestLogger>();
	AggregateLogger aLogger({ logger1, logger2 });
	aLogger.LogError(std::exception("Test"), "Test"s);
	int i = 0;
	while (i < 10 && !(logger1->IsErrorCalled())) {
		std::this_thread::sleep_for(100ms);
		++i;
	}

	EXPECT_TRUE(logger1->IsErrorCalled());
	EXPECT_TRUE(logger2->IsErrorCalled());
}