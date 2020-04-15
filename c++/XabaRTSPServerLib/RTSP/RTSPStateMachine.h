#pragma once
#include "RTSPMessageType.h"
class RTSPStateMachine{
public:
	virtual std::unique_ptr<RTSPStateMachine> Option(RTSPMessageOPTIONS& msg) = 0;
	virtual std::unique_ptr<RTSPStateMachine>  SetUp(const RTSPMessageSETUP& msg) = 0;
	virtual std::unique_ptr<RTSPStateMachine>  Play(const RTSPMessagePLAY& msg) = 0;

};

class RTSPInitStateMachine : public RTSPStateMachine {
	virtual std::unique_ptr<RTSPStateMachine> Option(RTSPMessageOPTIONS& msg) override;
	virtual std::unique_ptr<RTSPStateMachine>  SetUp(const RTSPMessageSETUP& msg) override;
	virtual std::unique_ptr<RTSPStateMachine>  Play(const RTSPMessagePLAY& msg) override;
};

class RTSPPlayingStateMachine : public RTSPStateMachine {

};

class RTSPPausedStateMachine : public RTSPStateMachine {

};

class RTSPFinishedStateMachine : public RTSPStateMachine {

};
