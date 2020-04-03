#include "pch.h"
#include "LogMessageProcessor.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
void LogMessageProcessor::Worker()
{
	while (!_finished)
	{
		auto [context,  func] = _messages.pop();	
		if (_finished)  continue;
		func(context);
	}
}

LogMessageProcessor::LogMessageProcessor() : _worker(&LogMessageProcessor::Worker, this){
}

void LogMessageProcessor::AddMessage(const LogMessageContext& context, std::function<void(const LogMessageContext&)> func)
{
    _messages.push(std::make_pair(context, func));
}

void LogMessageProcessor::Stop()
{
    _messages.terminate();
	_finished = true;
}

LogMessageProcessor::~LogMessageProcessor(){
	Stop();
	if (_worker.joinable()) {
		_worker.join();
	}
}


