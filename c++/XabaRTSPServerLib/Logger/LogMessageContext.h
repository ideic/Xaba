#pragma once
#include <string>
#include <exception>
#include <optional>
struct LogMessageContext{
	std::string message;
	std::optional<std::exception> exception;
	std::string time;
	std::string threadId;
};

