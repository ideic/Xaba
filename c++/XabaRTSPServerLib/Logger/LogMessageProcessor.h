#pragma once
#include <list>
#include <functional>
#include <variant>
#include <atomic>
#include <optional>
#include <string>
#include "LogMessageContext.h"
#include <thread>
#include "BlockingQueue.h"

using messageType = std::pair<LogMessageContext, std::function<void(const LogMessageContext&)>>;
class LogMessageProcessor
{
private:
	BlockingQueue<messageType> _messages ;
	void Worker();
	std::atomic_bool _finished{false};
	std::thread _worker;
	void Stop();
public:
	LogMessageProcessor();
	void AddMessage(const LogMessageContext &context, std::function<void(const LogMessageContext&)> func);
	~LogMessageProcessor();
};

