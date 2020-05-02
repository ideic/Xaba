#include "pch.h"
#include "MessageProcessorRTSPMessage.h"
#include <RTSP\RTSPMessageParser.h>
#include <memory>
#include <variant>
#include <atomic>
#include  <typeinfo>
std::shared_ptr<RTSPStateMachine>  MessageProcessorRTSPMessage::GetStateMachineForced(const uint64_t key){
	
	{
		std::shared_lock l(_readerWriterLock);
		
		if (_stateMachines.find(key) != end(_stateMachines)) {
			return _stateMachines[key];
		}
		
	}
	std::unique_lock l(_readerWriterLock);
	auto result = std::make_shared<RTSPStateMachine>();
	_stateMachines.emplace(key, result);

	return result;
}


uint64_t MessageProcessorRTSPMessage::GetKey(const TCPArrivedNetworkPackage& pckg){
	uint64_t key{ 0 };
	const auto& src = pckg.GetSrc();

	const uint8_t* from = (const uint8_t*)&(src.sin_addr);
	const uint8_t* end = from + 4;
	std::copy(from, end, (uint8_t*)&key);
	key = key << 32;
	std::copy_n((uint8_t*)&(src.sin_port), sizeof(src.sin_port), (uint8_t*)&key);
	return key;
}

void MessageProcessorRTSPMessage::RemoveStateMachine(uint64_t key){
	std::unique_lock l(_readerWriterLock);
	_stateMachines.erase(key);
}

void MessageProcessorRTSPMessage::ProcessNetworkPackage(const TCPArrivedNetworkPackage& pckg){
	RTSPMessageParser parser;
	

	std::string message(begin(pckg.buffer), end(pckg.buffer));
	auto rtspMessage = parser.Parse(message);

	auto key = GetKey(pckg);

	auto stateMachine = GetStateMachineForced(key);

	rtspMessage->Visit(stateMachine, pckg);
	
	if (stateMachine->IsInFinalState()) {
		RemoveStateMachine(key);
	}
}
