#include "pch.h"
#include "MessageProcessorLogger.h"
#include <Logger/LoggerFactory.h>
void MessageProcessorLogger::LogNetworkPackage(const TCPArrivedNetworkPackage& pckg){
	std::string message(begin(pckg.buffer), end(pckg.buffer));
	LOGGER->LogInfo(message);
}
