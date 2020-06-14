#include "AudioController.hpp"
#include "Receiver.hpp"
#include "msgqueue.hpp"
#include <thread>
#include <iostream>

void testReceiver(MsgQueue&);

int main(){
    MsgQueue msgQueue;
    int counter = 0;
    auto thread1 = std::thread(testReceiver, std::ref(msgQueue));
    for (;;) {
        msgQueue.enqueue(Message(HGET, counter));
        msgQueue.enqueue(Message(HSND, "Hello world " + std::to_string(counter)));
        counter++;
        if (counter >= 16000) {
            counter = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void testReceiver(MsgQueue & mq){
    for (;;) {
        Message message = mq.dequeue();
		switch (message.header) {
		case HGET:
			std::cout << "Numeric: " << message.numeric << std::endl;
			break;
		case HSND:
			std::cout << "Text: " << message.text << std::endl;
			break;
		}
	}
}