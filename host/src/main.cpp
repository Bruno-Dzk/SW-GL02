#include "AudioController.hpp"
#include "Receiver.hpp"
#include "Transmitter.hpp"
#include "msgqueue.hpp"
#include <thread>
#include <iostream>
#include <atomic>

int main(){
    std::atomic<bool> is_running(true);
    MsgQueue to_send_queue;
    MsgQueue received_queue;
    Transmitter transmitter(to_send_queue, is_running);
    Receiver receiver(received_queue, is_running);
    int counter = 0;
    for (;;) {
        Message test(ASND, counter);
        to_send_queue.enqueue(test);
        test = received_queue.dequeue();
        std::cout << test.numeric << "\n";
        counter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
    is_running.store(false);
}