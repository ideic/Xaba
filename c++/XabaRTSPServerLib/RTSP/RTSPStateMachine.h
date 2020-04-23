#pragma once
#include "RTSPMessageType.h"

//TODO Context should containnstateMachine, that will be overwritten at ChangeState
struct RTSPStateMachineContext {
	uint32_t Seq{ 0 };
};
class RTSPStateMachine{
protected:
	RTSPStateMachineContext _ctx;
public:
	virtual std::unique_ptr<RTSPStateMachine> Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) = 0;
	virtual std::unique_ptr<RTSPStateMachine>  SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) = 0;
	virtual std::unique_ptr<RTSPStateMachine>  Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) = 0;
	void CaptureCtx(RTSPStateMachineContext &ctx);
};

class RTSPInitStateMachine : public RTSPStateMachine {
	virtual std::unique_ptr<RTSPStateMachine> Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) override;
	virtual std::unique_ptr<RTSPStateMachine>  SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) override;
	virtual std::unique_ptr<RTSPStateMachine>  Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) override;
};

class RTSPPlayingStateMachine : public RTSPStateMachine {

};

class RTSPPausedStateMachine : public RTSPStateMachine {

};

class RTSPFinishedStateMachine : public RTSPStateMachine {

};
