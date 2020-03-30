#include <pch.h>
#include "LoggerFactory.h"
#include <Singleton.h>
std::shared_ptr<AggregateLogger> LoggerFactory::_logger = std::shared_ptr<AggregateLogger>( new AggregateLogger(
	{ std::make_shared<ConsoleLogger>() }
));

std::shared_ptr<Logger> LoggerFactory::Instance(){
	return _logger;
}

void LoggerFactory::AddLogger(std::shared_ptr<Logger> logger){
	_logger->AddLogger(logger);
}

