#include "pch.h"
#include "MessageProcessorRTSPMessage.h"
#include <RTSP\RTSPMessageParser.h>
#include <memory>
#include <variant>
#include <atomic>

void MessageProcessorRTSPMessage::CreateStateMachineIfItIsNotExists(const uint64_t key){

	std::atomic_bool found{ false };
	{
		std::shared_lock l(_readerWriterLock);
		found = _stateMachines.find(key) == end(_stateMachines);
	}
	if (!found) {
		std::unique_lock l(_readerWriterLock);
		_stateMachines.emplace(key, new RTSPInitStateMachine()); // TODO initStateMachine
	}
}

std::unique_ptr<RTSPStateMachine> MessageProcessorRTSPMessage::GetStateMachine(const uint64_t key){
	std::shared_lock l(_readerWriterLock);
	return std::move(_stateMachines[key]);
}

uint64_t MessageProcessorRTSPMessage::GetKey(const TCPArrivedNetworkPackage& pckg){
	uint64_t key{ 0 };
	const auto& src = pckg.GetSrc();

	std::copy_n((uint8_t*)&(src.sin_addr), 4, &key);
	key = key << 32;
	std::copy_n((uint8_t*)&(src.sin_port), sizeof(src.sin_port), &key);
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

	if (typeid(rtspMessage.get()) == typeid(RTSPMessageINVALID*)) {
		return;
	}
	auto key = GetKey(pckg);
	if (typeid(rtspMessage.get()) == typeid(RTSPMessageOPTIONS)) {
		CreateStateMachineIfItIsNotExists(key);
	}
	
	auto stateMachine = GetStateMachine(key);
	if (stateMachine == nullptr) return;

	{
		std::unique_lock l(_readerWriterLock);
		_stateMachines[key] = rtspMessage->Visit(std::move(stateMachine));
	}
	
	if (typeid(rtspMessage.get()) == typeid(RTSPMessageTEARDOWN)) {
		RemoveStateMachine(key);
	}
}
