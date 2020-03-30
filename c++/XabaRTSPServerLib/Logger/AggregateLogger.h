#pragma once
#include <vector>
#include <memory>
#include "Logger.h"
class AggregateLogger : public Logger
{
private:
	std::vector<std::shared_ptr<Logger>> _loggers;
protected:
	virtual void LogWarningCore(const LogMessageContext& context) override;
	virtual void LogInfoCore(const LogMessageContext& context) override;
	virtual void LogErrorCore(const LogMessageContext& context) override;

public:
	AggregateLogger(std::initializer_list<std::shared_ptr<Logger>> loggers);
	void AddLogger(std::shared_ptr<Logger> logger);
};

