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
    std::thread receiver_thread;
    MsgQueue *receivedQueue;
    void receive();

public:
    Receiver(MsgQueue &);
};
