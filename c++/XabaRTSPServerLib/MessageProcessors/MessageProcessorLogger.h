#pragma once
#include <Network\NetworkPackage.h>
class MessageProcessorLogger{
public:
	void LogNetworkPackage(const TCPArrivedNetworkPackage& pckg);
};

