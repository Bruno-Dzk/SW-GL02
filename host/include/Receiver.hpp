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

class Receiver {
    MsgQueue *receivedQueue;
    std::thread receiver_thread;
    std::atomic<bool> *isRunning;
    int serialPort;

    void receive();

public:
    Receiver(MsgQueue &, std::atomic<bool> &);
    ~Receiver();
};