#pragma once
#include <string>
using namespace std::string_literals;
class RTSPMessageType{
private:
	
	virtual void ParseCore(std::string_view message) = 0;
	virtual const std::string Token() = 0;
protected:
	uint64_t _cseq{ 0 };
public:
	bool Parse(std::string_view message);
	uint64_t CSeq();
};

class RTSPMessageOPTIONS: public RTSPMessageType {
private:
	std::string _url{""s};
	std::string _version{ ""s };
	virtual const std::string Token()  override;
	virtual void ParseCore(std::string_view message) override;
public:
	const std::string& URL();
	const std::string& Version();


};

class RTSPMessageSETUP : public RTSPMessageType {
private:
	virtual void ParseCore(std::string_view message) override {};
	virtual const std::string Token()  override { return "SETUP"s; };
public:
	
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