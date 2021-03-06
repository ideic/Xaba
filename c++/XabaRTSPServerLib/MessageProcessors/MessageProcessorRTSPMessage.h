#pragma once
#include <Network\NetworkPackage.h>
#include <map>
#include <RTSP\RTSPStateMachine.h>
#include <shared_mutex>
class MessageProcessorRTSPMessage{
private:
	std::shared_mutex _readerWriterLock{};
	std::map<uint64_t, std::shared_ptr<RTSPStateMachine>> _stateMachines;
	std::shared_ptr<RTSPStateMachine>  GetStateMachineForced(const uint64_t key);
	uint64_t GetKey(const TCPArrivedNetworkPackage& pckg);
	void RemoveStateMachine(uint64_t key);
public:
	void ProcessNetworkPackage(const TCPArrivedNetworkPackage& pckg);
};

