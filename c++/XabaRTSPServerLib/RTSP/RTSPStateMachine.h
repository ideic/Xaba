#pragma once
#include "RTSPMessageType.h"

class RTSPState;

class RTSPStateMachineContext {
public:
	uint32_t Seq{ 0 };
};

class RTSPStateMachine {
private:
	RTSPStateMachineContext _ctx{};
	std::shared_ptr<RTSPState> _state;
public:
	RTSPStateMachine();
	RTSPStateMachineContext* Context() { return &_ctx; };
	std::shared_ptr<RTSPState> State();

	void ChangeState(const std::shared_ptr<RTSPState>& newState);

	void Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage);
	void SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage);
	void Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage);

	void Invalid(const RTSPMessageINVALID& msg, const TCPArrivedNetworkPackage& networkPackage);

	bool IsInFinalState();
};
