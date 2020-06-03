#pragma once
#include <string>
#include "msgqueue.hpp"

class Codec {
public:
	Codec();

	std::string Encode(Message msg);
	Message Decode(std::string frame);

	int Checksum(std::string data);
};