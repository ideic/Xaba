#include "pch.h"
#include "LogMessageProcessor.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
void LogMessageProcessor::Worker()
{
	while (!_finished)
	{
		std::optional<messageType> msg = _messages.Pop();
		if (!msg) {
			_finished = true;
			break;
		}
		auto &[context, func] = msg.value();
		if (_finished)  break;
		func(context);
	}
}

LogMessageProcessor::LogMessageProcessor() : _worker(&LogMessageProcessor::Worker, this){
}

void LogMessageProcessor::AddMessage(const LogMessageContext& context, std::function<void(const LogMessageContext&)> func)
{
    _messages.Push(std::make_pair(context, func));
}

void LogMessageProcessor::Stop()
{
    _finished = true;
    _messages.Terminate();
    if (_worker.joinable()) {
        _worker.join();
    }
}

LogMessageProcessor::~LogMessageProcessor(){
	Stop();
}

void LogMessageProcessor::Suspend(){
	_finished = true;
}


