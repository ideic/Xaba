#pragma once
#include <Logger/Logger.h>
class TestLogger : public Logger {
private:
	std::atomic_bool _isWarningCalled = {false};
	std::atomic_bool _isInfoCalled = { false };
	std::atomic_bool _isErrorCalled = { false };

protected:
	virtual void LogWarningCore(const LogMessageContext& context) override; 
	virtual void LogInfoCore(const LogMessageContext& context) override ;
	virtual void LogErrorCore(const LogMessageContext& context) override ;
public:
	bool IsWarningCalled();
	bool IsInfoCalled();
	bool IsErrorCalled();
};

