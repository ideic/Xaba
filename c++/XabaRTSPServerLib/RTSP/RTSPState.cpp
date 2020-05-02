#include <pch.h>
#include "RTSPState.h"
#include <BlockingQueue.h>
#include <Singleton.h>

void RTSPInitState::Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage) {
	auto origmsg = msg;
	std::string response = origmsg.Version();
	response += " 200 OK\r\n";
	response += "CSeq:" + std::to_string(_stateMachine->Context()->Seq++) + "\r\n";
	response += "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n";

	TCPResponseNetworkPackage responsePckg;

	responsePckg.buffer = std::vector<char>(response.begin(), response.end());
	responsePckg.SetDst(networkPackage.GetSrc());
	responsePckg.SetSrc(networkPackage.GetDst());
	responsePckg.socket = networkPackage.socket;

	Singleton<BlockingQueue<TCPResponseNetworkPackage>>().Push(std::move(responsePckg));
}

void RTSPInitState::SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage) {
	;
}

void RTSPInitState::Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage) {
	;
}

void RTSPState::Invalid(const RTSPMessageINVALID& msg, const TCPArrivedNetworkPackage& networkPackage) {
	_stateMachine->ChangeState(std::make_shared<RTSPFinished>(_stateMachine));
	_stateMachine->Invalid(msg, networkPackage);
};
