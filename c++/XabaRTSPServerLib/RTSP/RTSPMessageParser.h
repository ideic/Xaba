#pragma once
#include <string_view>
#include <memory>
#include "RTSPMessageType.h"

class RTSPMessageParser{
public:
	std::unique_ptr<RTSPMessageType> Parse(std::string_view message);
};

