#include "pch.h"
#include "RTSPMessageType.h"
#include <sstream>
#include <vector>
#include <string>;
#include <algorithm>

using namespace std::string_view_literals;
using namespace std::string_literals;

std::vector<std::string> Split(std::string_view source, const std::string& delimiter) {
	std::vector<std::string> result;

	std::size_t current = 0;
	std::size_t position = source.find_first_of(delimiter, 0);

	while (position != std::string::npos)
	{
		result.emplace_back(source, current, position - current);
		current = position + 1;
		position = source.find_first_of(delimiter, current);
	}

	result.emplace_back(std::string(source), current);

	return result;
}

bool RTSPMessageType::Parse(std::string_view message) {
	if (message.find_first_of(Token()) != std::string::npos) {
		ParseCore(message);
		return true;
	}

	return false;
}

uint64_t RTSPMessageType::CSeq(){
	return _cseq;
}

const std::string RTSPMessageOPTIONS::Token() { return "OPTIONS"s; }

void RTSPMessageOPTIONS::ParseCore(std::string_view message){

	auto lines = Split(message, "\r\n"s);
	
	auto firstline = lines.front();

	auto firstLineTokens = Split(firstline, " "s);

	for (auto &token : firstLineTokens) {
		if (token._Starts_with("rtsp://"sv) || token._Starts_with("*"sv)) {
			_url = token;
		}
		else if (token._Starts_with("RTSP/"sv)) {
			_version = token;
		}
	}

	std::for_each(begin(lines) + 1, end(lines), [this](const std::string& line) {
		if (line._Starts_with("CSeq"s)) {
			_cseq = std::stoi(std::string(line.substr("CSeq:"s.length(), line.length() - "CSeq:"s.length())));
			return;
		}
	});
}

const std::string& RTSPMessageOPTIONS::URL(){
	return _url;
}

const std::string& RTSPMessageOPTIONS::Version(){
	return _version;
}
