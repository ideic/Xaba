#include "pch.h"
#include "RTSPMessageType.h"
#include <sstream>
using namespace std::string_view_literals;
using namespace std::string_literals;

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
	auto firstline = message.substr(Token().size() + 1, message.find_first_of("\r") - (Token().size()-1));
	size_t currentPos = 0;
	size_t nextPos = 0;
	while (currentPos < firstline.length()) {
		nextPos = firstline.find_first_of(" "s, currentPos+1);
		std::string_view token;
		if (nextPos != std::string::npos) {
			token = firstline.substr(currentPos, nextPos - currentPos);
		}
		else {
			token = firstline.substr(currentPos+1, firstline.length()-currentPos-1-2); // \r \n
		}
		if (token._Starts_with("rtsp://"sv) || token._Starts_with("*"sv)) {
			_url = token;
		}
		else if (token._Starts_with("RTSP/"sv)) {
			_version = token;
		}

		currentPos = nextPos;

	}

	if (message.size() == firstline.size()) return;

	currentPos = firstline.length() + Token().size() + 1;
	while (currentPos < message.length()){
		auto returnPos = message.find_first_of("\r", currentPos);
		if (returnPos == std::string::npos) return;

		auto nextline = message.substr(currentPos, returnPos - (currentPos));
		if (nextline._Starts_with("CSeq"sv)) {
			_cseq = std::stoi(std::string(nextline.substr("CSeq:"s.length(), nextline.length() - "CSeq:"s.length())));
			return;
		}
		currentPos += (nextline.length() == 0 ? 1 : nextline.length());
	}


}

const std::string& RTSPMessageOPTIONS::URL(){
	return _url;
}

const std::string& RTSPMessageOPTIONS::Version(){
	return _version;
}
