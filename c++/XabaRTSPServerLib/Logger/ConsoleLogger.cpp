#include <pch.h>
#include "ConsoleLogger.h"
#include <string>
#include <iostream>
using namespace std::string_literals;
void ConsoleLogger::LogWarningCore(const LogMessageContext& context)
{
	std::cout << "[WARNING]" << context.threadId << std::string(" > ") << context.time << std::string(" | ") << context.message << "\n"s;
}

void ConsoleLogger::LogInfoCore(const LogMessageContext& context)
{
	std::cout << "[INFO]" << context.threadId << std::string(" > ") << context.time << std::string(" | ") << context.message << "\n"s;
}

void ConsoleLogger::LogErrorCore(const LogMessageContext& context)
{
	std::cerr << "[ERROR]" << context.threadId << std::string(" > ") << context.time << std::string(" | ") << "Message:" << context.message << "Exception:" << context.exception.value().what() << "\n"s;
	std::cout << "[ERROR]" << context.threadId << std::string(" > ") << context.time << std::string(" | ") << "Message:" << context.message << "Exception:" << context.exception.value().what() << "\n"s;

}

