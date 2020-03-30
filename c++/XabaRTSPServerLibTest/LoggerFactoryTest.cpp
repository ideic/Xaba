#include "pch.h"
#include <Logger/LoggerFactory.h>
#include <Singleton.h>
#include <chrono>
#include "TestLogger.h"
using namespace std::chrono_literals;

TEST(LoggerFactoryTest, LoggerIsNotNull) {
   EXPECT_NE(LOGGER, nullptr);
}

TEST(LoggerFactoryTest, AddLoggerCallsNewLogger) {
	std::shared_ptr<TestLogger> testLogger (new TestLogger());
	LoggerFactory::AddLogger(testLogger);
	LOGGER->LogWarning("Test");
	int i = 0;
	while (i < 10 && !testLogger->IsWarningCalled()) {
		std::this_thread::sleep_for(100ms);
		++i;
	}
	
    EXPECT_TRUE(testLogger->IsWarningCalled());
}