#include "pch.h"
#include "RTSPStateMachine.h"
#include "RTSPState.h"

RTSPStateMachine::RTSPStateMachine(): _state(new RTSPInitState(&(*this))){
}

std::shared_ptr<RTSPState> RTSPStateMachine::State() {
	return _state;
}
void RTSPStateMachine::ChangeState(const std::shared_ptr<RTSPState>& newState) {
	_state = newState;
}
void RTSPStateMachine::Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) {
	_state->Option(msg, networkPackage);
}
void RTSPStateMachine::SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) {
	_state->SetUp(msg, networkPackage);
}
void RTSPStateMachine::Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) {
	_state->Play(msg, networkPackage);
}

void RTSPStateMachine::Invalid(const RTSPMessageINVALID& msg, const TCPArrivedNetworkPackage& networkPackage) {
	_state->Invalid(msg, networkPackage);
}

bool RTSPStateMachine::IsInFinalState() {
	return typeid(*_state.get()).hash_code() == typeid(RTSPFinished).hash_code(); 
}
