#include "pch.h"
#include "TestLogger.h"

void TestLogger::LogWarningCore(const LogMessageContext& context){
	_isWarningCalled = true;
}

void TestLogger::LogInfoCore(const LogMessageContext& context){
	_isInfoCalled = true;
}

void TestLogger::LogErrorCore(const LogMessageContext& context){
	_isErrorCalled = true;
}

bool TestLogger::IsWarningCalled(){
	return _isWarningCalled;
}

bool TestLogger::IsInfoCalled() {
	return _isInfoCalled;
}

bool TestLogger::IsErrorCalled() {
	return _isErrorCalled;
}
