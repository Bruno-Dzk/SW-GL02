#include "Codec.hpp"
#include <iostream>

#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? sum += 1 : sum), \
  (byte & 0x40 ? sum += 1 : sum), \
  (byte & 0x20 ? sum += 1 : sum), \
  (byte & 0x10 ? sum += 1 : sum), \
  (byte & 0x08 ? sum += 1 : sum), \
  (byte & 0x04 ? sum += 1 : sum), \
  (byte & 0x02 ? sum += 1 : sum), \
  (byte & 0x01 ? sum += 1 : sum) 

//Array of the headers as string 
static const std::string headers[] = {
	"HRDY",
	"ERDY",
	"ASET",
	"ASND",
	"CSND",
	"RSND",
	"TSND",
	"KEYP",
	"VSND",
	"HGET",
	"HSND"
};

//Array of masks  to helps convert unsigned int to char array
static const unsigned int masks[] = { 
	0b1111111,
	0b11111110000000,
	0b111111100000000000000,
	0b1111111000000000000000000000,
	0b11110000000000000000000000000000
};

//Default constructor
Codec::Codec()
{

}

std::string Codec::Encode(Message message)
{
	/*
		The function accepts message type data and encodes in a string
		Adds a start byte and checksum to the frame
	*/
	std::string data = "";

	if (headers[message.header] == "HSND") {
		data = char(message.text.length()) + message.text;
	}
	else {
		unsigned char bytes[5];
		//conversion of uint to char array
		bytes[0] = message.numeric & masks[0];
		bytes[1] = (message.numeric & masks[1]) >> 7;
		bytes[2] = (message.numeric & masks[2]) >> 14;
		bytes[3] = (message.numeric & masks[3]) >> 21;
		bytes[4] = (message.numeric & masks[4]) >> 28;

		for (int i = 0; i < 5; i++) {
			data += bytes[i];
		}
	}

	return char(255) + headers[message.header] + data + char(Checksum(headers[message.header] + data));
}

Message Codec::Decode(std::string frame) 
{
	/*
		The function accepts data as a string and decodes into the Message type
	*/
	if (frame.substr(1, 4) == "HSND") {
		return Message(HSND, frame.substr(6, int(frame[5])));
	}
	else {
		int i = 0;
		unsigned int data = 0;

		while (frame.substr(1, 4) != headers[i]) {
			i++;
		}
		//conversion of char array to uint
		data |= (frame[9] & masks[0]) << 28;
		data |= (frame[8] & masks[0]) << 21;
		data |= (frame[7] & masks[0]) << 14;
		data |= (frame[6] & masks[0]) << 7;
		data |= (frame[5] & masks[0]);
		
		return Message((MsgType)i, data);
	}
}

int Codec::Checksum(std::string data)
{
	//Counting the checksum from a string of data
	int sum = 0;
	for (int i = 0; i < data.length(); i++) {
		BYTE_TO_BINARY(data[i]);
	}

	return sum;
}
