#include "pch.h"
#include "RTSPMessageType.h"
#include <sstream>
#include <vector>
#include <string>;
#include <algorithm>
#include "RTSPStateMachine.h"
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

void RTSPMessageType::ParseCore(std::string_view message){
	auto lines = Split(message, "\r\n"s);

	auto firstline = lines.front();

	auto firstLineTokens = Split(firstline, " "s);

	for (auto& token : firstLineTokens) {
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

bool RTSPMessageType::Parse(std::string_view message) {
	if (message._Starts_with(Token())) {
		ParseCore(message);
		return true;
	}

	return false;
}

uint64_t RTSPMessageType::CSeq(){
	return _cseq;
}

const std::string& RTSPMessageType::URL() {
	return _url;
}

const std::string& RTSPMessageType::Version() {
	return _version;
}

const std::string RTSPMessageOPTIONS::Token() { return "OPTIONS"s; }

void RTSPMessageOPTIONS::ParseCore(std::string_view message){
	RTSPMessageType::ParseCore(message);
	
}

std::unique_ptr<RTSPStateMachine> RTSPMessageOPTIONS::Visit(std::unique_ptr<RTSPStateMachine> stateMachine){
	return stateMachine->Option(*this);
}

void RTSPMessageSETUP::ParseCore(std::string_view message){
	RTSPMessageType::ParseCore(message);
	auto lines = Split(message, "\r\n");

	auto found = std::find_if(begin(lines), end(lines), [](const std::string line) {
		return line._Starts_with("Transport:");
		});
	if (found != end(lines)) {
		_transport.Raw = *found;
		auto transportLines = Split(*found, ";");
		for (auto& transportLine : transportLines) {
			if (transportLine._Starts_with("Transport: RTP"s)) {
				_transport.TransportProtocol = "RTP"s;
				auto sep = transportLine.find_first_of("/"s);
				_transport.TransportProfile = transportLine.substr(sep+1, transportLine.length() - sep);
			}
			else if (transportLine._Starts_with("unicast"s)) {
				_transport.IsUnicast = true;
			}
			else if (transportLine._Starts_with("client_port"s)) {
				auto portStart = transportLine.find_first_of("=");
				auto port2Start = transportLine.find_first_of("-");
				_transport.clientportRTP = stoi( transportLine.substr(
					portStart + 1,
					port2Start - portStart));
				_transport.clientportRTCP = stoi(
					transportLine.substr(port2Start + 1, transportLine.length() - port2Start));

			}

		}
			
	}
}

const std::string RTSPMessageSETUP::Token() { return "SETUP"s; }

std::unique_ptr<RTSPStateMachine> RTSPMessageSETUP::Visit(std::unique_ptr<RTSPStateMachine> stateMachine){
	return stateMachine->SetUp(*this);
}

RTSPMessageSETUP::SetupTransport RTSPMessageSETUP::Transport(){
	return _transport;
}
