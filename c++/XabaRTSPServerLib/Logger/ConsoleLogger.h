#pragma once
#include "Logger.h"
class ConsoleLogger : public Logger
{
protected:
	virtual void LogWarningCore(const LogMessageContext& context) override;
	virtual void LogInfoCore(const LogMessageContext& context) override;
	virtual void LogErrorCore(const LogMessageContext& context) override;
};

