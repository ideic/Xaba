#include "pch.h"
#include "LogMessageProcessor.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
void LogMessageProcessor::Worker()
{
	while (!_finished)
	{
		while (_messages.empty()) {
			std::this_thread::sleep_for(100ms);
			if (_finished) return;
		}
		auto [context,  func] = _messages.front();
		_messages.pop_front();
		
		func(context);
	}
}

LogMessageProcessor::LogMessageProcessor() : _worker(&LogMessageProcessor::Worker, this){
}

void LogMessageProcessor::AddMessage(const LogMessageContext& context, std::function<void(const LogMessageContext&)> func)
{
	_messages.emplace_back( context, func );
}

void LogMessageProcessor::Stop()
{
	_finished = true;
}

LogMessageProcessor::~LogMessageProcessor(){
	Stop();
	if (_worker.joinable()) {
		_worker.join();
	}
}


