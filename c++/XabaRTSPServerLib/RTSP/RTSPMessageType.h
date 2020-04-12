#pragma once
#include <string>
using namespace std::string_literals;
class RTSPMessageType{
protected:
	
	virtual void ParseCore(std::string_view message);
	virtual const std::string Token() = 0;
protected:
	uint64_t _cseq{ 0 };
	std::string _url{ ""s };
	std::string _version{ ""s };

public:
	bool Parse(std::string_view message);
	uint64_t CSeq();
	const std::string& URL();
	const std::string& Version();

};

class RTSPMessageOPTIONS: public RTSPMessageType {
private:
	virtual const std::string Token()  override;
	virtual void ParseCore(std::string_view message) override;
public:

};

class RTSPMessageSETUP : public RTSPMessageType {

protected:
	virtual void ParseCore(std::string_view message) override;
	virtual const std::string Token()  override { return "SETUP"s; };
public:
	struct SetupTransport {
		std::string Raw;
		bool IsUnicast{ false };
		std::string TransportProtocol;
		std::string TransportProfile; //Audio Video Profile

		uint16_t clientportRTP{0};
		uint16_t clientportRTCP{0};
	};

	RTSPMessageSETUP::SetupTransport Transport();
private:
	RTSPMessageSETUP::SetupTransport _transport{};
};
class RTSPMessagePLAY : public RTSPMessageType {
};
class RTSPMessageRECORD : public RTSPMessageType {
};
class RTSPMessagePAUSE : public RTSPMessageType {
};
class RTSPMessageTEARDOWN : public RTSPMessageType {
};
class RTSPMessageDESCRIBE : public RTSPMessageType {
};
class RTSPMessageANNOUNCE : public RTSPMessageType {
};
class RTSPMessageGET_PARAMETER : public RTSPMessageType {
};
class RTSPMessageREDIRECT : public RTSPMessageType {
};
class RTSPMessageSET_PARAMETER : public RTSPMessageType {
};

class RTSPMessageINVALID : public RTSPMessageType {
};

//f an RTSP agent does not support a particular method, it MUST return 501 (Not Implemented) and t