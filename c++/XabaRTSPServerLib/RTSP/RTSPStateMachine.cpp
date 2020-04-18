#include "pch.h"
#include "RTSPStateMachine.h"
#include "Network/NetworkPackage.h"
#include "Singleton.h"
#include "BlockingQueue.h"

std::unique_ptr<RTSPStateMachine> RTSPInitStateMachine::Option(const RTSPMessageOPTIONS& msg, const TCPArrivedNetworkPackage& networkPackage){
	auto origmsg = msg;
	std::string response = origmsg.Version();
	response += " 200 OK\r\n";
	response += "CSeq:" + std::to_string(_ctx.Seq++) + "\r\n";
	response += "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n";

	TCPResponseNetworkPackage responsePckg;

	responsePckg.buffer = std::vector<char>(response.begin(), response.end());
	responsePckg.SetDst(networkPackage.GetSrc());
	responsePckg.SetSrc(networkPackage.GetDst());

	Singleton<BlockingQueue<TCPResponseNetworkPackage>>().Push(std::move(responsePckg));
	//return std::make_unique<RTSPInitStateMachine>(this);
	std::unique_ptr<RTSPInitStateMachine> result = std::make_unique<RTSPInitStateMachine>();
	result->CaptureCtx(_ctx);
	return result;
}

std::unique_ptr<RTSPStateMachine> RTSPInitStateMachine::SetUp(const RTSPMessageSETUP& msg, const TCPArrivedNetworkPackage& networkPackage)
{
	return std::unique_ptr<RTSPStateMachine>();
}

std::unique_ptr<RTSPStateMachine> RTSPInitStateMachine::Play(const RTSPMessagePLAY& msg, const TCPArrivedNetworkPackage& networkPackage)
{
	return std::unique_ptr<RTSPStateMachine>();
}

void RTSPStateMachine::CaptureCtx(RTSPStateMachineContext &ctx){
	_ctx = std::move(ctx);
}
