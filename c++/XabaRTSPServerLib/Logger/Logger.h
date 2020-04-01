#pragma once
#include <string>
#include <list>
#include <thread>
#include <variant>
#include <atomic>
#include "LogMessageProcessor.h"
#include <Singleton.h>
class Logger {
private:
	LogMessageProcessor &_engine = Singleton<LogMessageProcessor>();
	std::string ThreadId();
	std::string Now();
protected:
	virtual void LogWarningCore(const LogMessageContext& context) {};
	virtual void LogInfoCore(const LogMessageContext& context)=0;
	virtual void LogErrorCore(const LogMessageContext& context)=0;

public:
	void LogWarning(const std::string& message);
	void LogInfo(const std::string& message);
	void LogError(const std::exception& exception, const std::string& message);
};


