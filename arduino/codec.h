#pragma once
#include "message.h"

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
static const unsigned char headers[][5] = {
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

//Array of masks  to helps convert unsigned long to char array
static const unsigned long masks[] = { 
	0b1111111,
	0b11111110000000,
	0b111111100000000000000,
	0b1111111000000000000000000000,
	0b11110000000000000000000000000000
};

byte checksum(byte *, size_t);
byte * codec_encode(Message &, size_t&);
Message codec_decode(byte *, size_t, byte *, size_t);
/////////////////////////////////////////////////////////////////////////////////

byte * codec_encode(Message & message, size_t & encoded_size)
{
	/*
		The function accepts message type data and encodes in a string
		Adds a start byte and checksum to the frame
	*/
  byte * data;
  size_t datasize;

	if (memcmp(headers[message.header],"HSND",4) == 0) {
    /*datasize = 1 + message.text.length();
    data = new byte [datasize];
    data[0] = message.text.length();
    for(int i = 1; i < datasize; i++){
       data[i] = message.text[i];
    }*/
    data = (byte)message.text;
	}
	else {
    datasize = 5;
		data = new byte[datasize];
		//conversion of uint to char array
		data[0] = message.numeric & masks[0];
		data[1] = (message.numeric & masks[1]) >> 7;
		data[2] = (message.numeric & masks[2]) >> 14;
		data[3] = (message.numeric & masks[3]) >> 21;
		data[4] = (message.numeric & masks[4]) >> 28;
	}

  byte * header = new byte[4];
  for(int i = 0; i < 4; i++){
    header[i] = byte(headers[message.header][i]);
  }

  encoded_size = 1 + 4 + datasize + 1;
  byte * encoded = new byte[encoded_size];
  encoded[0] = 254;
  memcpy(encoded + 1, header, 4);
  memcpy(encoded + 5, data, datasize);
  byte * to_checksum = new byte[datasize + 4];
  memcpy(to_checksum, header, 4);
  memcpy(to_checksum + 4, data, datasize);
  delete [] header;
  /*delete [] data;*/
  encoded[encoded_size - 1] = checksum(to_checksum, datasize + 4);
  delete [] to_checksum;
  delete [] data;

	return encoded;
}

Message codec_decode(byte * header, byte * data, size_t datasize) 
{
	/*
		The function accepts data as a string and decodes into the Message type
	*/
  Message decoded;
	if (memcmp(header,"HSND",4) == 0) {
    decoded.header = HSND;
    decoded.text = (char*)data;
	}
	else{
		int i = 0;
		unsigned long numeric = 0;

		while (memcmp(header,headers[i],4) != 0) {
			i++;
		}
		//conversion of char array to uint
		numeric |= (data[4] & masks[0]) << 28;
		numeric |= (data[3] & masks[0]) << 21;
		numeric |= (data[2] & masks[0]) << 14;
		numeric |= (data[1] & masks[0]) << 7;
		numeric |= (data[0] & masks[0]);

    decoded.header = (MsgType)i;
    decoded.numeric = numeric;
	}
  return decoded;
}

byte checksum(byte * data, size_t datasize)
{
	//Counting the checksum from a string of data
	int sum = 0;
	for (int i = 0; i < datasize; i++) {
		BYTE_TO_BINARY(data[i]);
	}

	return sum;
}
