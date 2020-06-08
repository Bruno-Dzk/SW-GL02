#pragma once
#include <string>
#include "msgqueue.hpp"

class Codec {
public:
	Codec();
	
	//Functions to encodes and decodes Messages
	std::string Encode(Message msg);
	Message Decode(std::string frame);
	
	//Function to counting checksum
	int Checksum(std::string data);
};
