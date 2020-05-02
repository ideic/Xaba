#pragma once
#include "RTSPStateMachine.h"

class RTSPState {
protected:
	RTSPStateMachine* _stateMachine;
public:
	RTSPState(RTSPStateMachine* stateMachine) : _stateMachine(stateMachine) {};
	virtual void Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) = 0;
	virtual void SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) = 0;
	virtual void Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) = 0;
	virtual void Invalid(const RTSPMessageINVALID& msg, const TCPArrivedNetworkPackage& networkPackage);
};

class RTSPInitState : public RTSPState {
public:
	RTSPInitState(RTSPStateMachine* stateMachine) : RTSPState(stateMachine) {};
	virtual void Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) override;
	virtual void  SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) override;
	virtual void  Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) override;
};

class RTSPPlayingState : public RTSPState {

};

class RTSPPausedState : public RTSPState {

};

class RTSPFinished : public RTSPState {
public:
	RTSPFinished(RTSPStateMachine* stateMachine) : RTSPState(stateMachine) {};
	virtual void Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) override {};
	virtual void  SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) override {};
	virtual void  Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) override {};
	virtual void Invalid(const RTSPMessageINVALID& msg, const TCPArrivedNetworkPackage& networkPackage) override {};
};
