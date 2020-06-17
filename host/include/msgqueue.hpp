#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

enum MsgType{
	HRDY,
	ERDY,
	ASET,
	ASND,
	CSND,
	RSND,
	TSND,
	KEYP,
	VSND,
	HGET,
	HSND
};

struct Message {
	MsgType header;
	unsigned int numeric;
	std::string text;
	Message(MsgType, unsigned int);
	Message(MsgType, std::string);
};

class MsgQueue
{
public:
	MsgQueue(void);
	~MsgQueue(void);
	// Add an element to the queue.
	void enqueue(Message);
	// Get the "front"-element.
	// If the queue is empty, wait till a element is avaiable.
	Message dequeue(void);
	bool empty(void);
private:
	std::queue<Message> queue;
	mutable std::mutex mutex;
	std::condition_variable convar;
};