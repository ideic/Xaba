#include <pch.h>
#include "AggregateLogger.h"


AggregateLogger::AggregateLogger(std::initializer_list<std::shared_ptr<Logger>> loggers){
	for (auto& logger : loggers) {
		_loggers.push_back(logger);
	}
}

void AggregateLogger::LogWarningCore(const LogMessageContext& context){
	for (auto& logger : _loggers) {
		logger->LogWarning(context.message);
	}
}

void AggregateLogger::LogInfoCore(const LogMessageContext& context){
	for (auto& logger : _loggers) {
		logger->LogInfo(context.message);
	}
}

void AggregateLogger::LogErrorCore(const LogMessageContext &context){
	for (auto& logger : _loggers) {
		logger->LogError(context.exception.value(), context.message);
	}
}

void AggregateLogger::AddLogger(std::shared_ptr<Logger> logger){
	_loggers.push_back(logger);
}