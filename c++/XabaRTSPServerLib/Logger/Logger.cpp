#include <pch.h>
#include "Logger.h"
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <functional>
void Logger::LogWarning(const std::string& message){
	_engine.AddMessage({ message, std::nullopt, Now(), ThreadId() }, std::bind(&Logger::LogWarningCore, this, std::placeholders::_1));
}

void Logger::LogInfo(const std::string& message){
	_engine.AddMessage({ message, std::nullopt, Now(), ThreadId() }, std::bind(&Logger::LogInfoCore, this, std::placeholders::_1));
}

void Logger::LogError(const std::exception& exception, const std::string& message){
	_engine.AddMessage({ message, exception, Now(), ThreadId() }, std::bind(&Logger::LogErrorCore, this, std::placeholders::_1));
}

std::string Logger::ThreadId()
{
	std::ostringstream oss;
	oss << "[" << std::this_thread::get_id() << "]";
	return oss.str();
}

std::string Logger::Now()
{
	auto now = std::chrono::system_clock::now();
	auto now2 = std::chrono::system_clock::to_time_t(now);

	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

	auto fractional_seconds = millisec.count() % 1000;

	struct tm buf;
	auto localTime = localtime_s(&buf, &now2);

	std::ostringstream oss;
	oss << std::put_time(&buf, "%Y.%m.%d %H:%M:%S.") << fractional_seconds;
	return oss.str();

}