#pragma once
#include <memory>
#include <string>
#include "Logger.h"
#include "AggregateLogger.h"
#include "ConsoleLogger.h"


class LoggerFactory
{
private:
	static std::shared_ptr<AggregateLogger> _logger;
public:
	static std::shared_ptr<Logger> Instance();

	static void AddLogger(std::shared_ptr<Logger> logger);
};

#define LOGGER LoggerFactory::Instance()
