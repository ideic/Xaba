#include "pch.h"
#include "RTSPMessageParser.h"
using namespace std::string_literals;

template<class MessageType> std::unique_ptr<MessageType> Match(std::string_view message) {
	auto result = std::make_unique<MessageType>();
	if (result->Parse(message))
		return result;

	return  nullptr;
}

std::unique_ptr<RTSPMessageType> RTSPMessageParser::Parse(std::string_view message)
{
	std::unique_ptr<RTSPMessageType> result;
	if ( (result = Match<RTSPMessageOPTIONS>(message)) != nullptr) return result;

	if ((result = Match<RTSPMessageSETUP>(message)) != nullptr) return result;

	/*if ((result = Match<RTSPMessagePLAY>(message, "PLAY"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageRECORD>(message, "RECORD"s)) != nullptr) return result;

	if ((result = Match<RTSPMessagePAUSE>(message, "PAUSE"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageTEARDOWN>(message, "TEARDOWN"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageDESCRIBE>(message, "DESCRIBE"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageANNOUNCE>(message, "PLAY"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageGET_PARAMETER>(message, "PLAY"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageREDIRECT>(message, "RECORD"s)) != nullptr) return result;

	if ((result = Match<RTSPMessageSET_PARAMETER>(message, "PAUSE"s)) != nullptr) return result;*/

	return std::unique_ptr<RTSPMessageINVALID>();
}

