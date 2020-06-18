#pragma once
#include <iostream>
#include <stdio.h>
#include <thread>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "Codec.hpp"
#include "msgqueue.hpp"
#include <atomic>

// a transmitter class. 

// considering the fact, that the loop in the member thread if infinite,
// we should have the way to stop it, otherwise, even if 
// the .join() function is called, the thread wont finish.

class Transmitter {

	// transmitter thread-member
	std::thread transmitter_thread;

	// pointer to a message queue
	MsgQueue *toSendQueue;

	// thread function itself
	void send();

	// serial port file descriptor
	int serial_port_fd;

	// an atomic boolean variable, that has to be passed using std::ref 
	// to the class constructor from the main function in order to control the 
	// workflow of the thread
	std::atomic<bool> *program_status;
	std::atomic<bool> *arduino_running;

public:
	// param. constructor
	Transmitter(MsgQueue &, std::atomic<bool>&);

	// destructor
	~Transmitter();
};
