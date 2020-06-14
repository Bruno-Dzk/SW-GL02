#include "AudioController.hpp"
#include "PerformanceMonitor.hpp"
#include "Receiver.hpp"
#include "Transmitter.hpp"
#include "msgqueue.hpp"
#include <thread>
#include <iostream>
#include <atomic>
#include <chrono>

void outcoming_thread_function(MsgQueue & to_send_queue){
    PerformanceMonitor performance_monitor;
    auto t1 = std::chrono::high_resolution_clock::now(); 
    double perfmon_acc = 0.0;
    double audio_acc = 0.0;
    for (;;) {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> delta = t2 - t1;

        if(audio_acc > 30){
            AudioController audio_controller;
            Message audio_info(ASND, audio_controller.getLevel());
            to_send_queue.enqueue(audio_info);
            audio_acc = 0.0;
        }
       
        if(perfmon_acc > 100.0){
            Message cpu_info(CSND, performance_monitor.getCPUUsage());
            to_send_queue.enqueue(cpu_info);
            Message ram_info(RSND, performance_monitor.getRAMUsage());
            to_send_queue.enqueue(ram_info);
            Message temp_info(TSND, performance_monitor.getCPUTemp());
            to_send_queue.enqueue(temp_info);
            Message testmes(ASET, 1024);
            to_send_queue.enqueue(testmes);
            perfmon_acc = 0.0;
        }
        perfmon_acc += delta.count();
        audio_acc += delta.count();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        t1 = t2;
	}
}

int main(){
    std::atomic<bool> is_running(true);
    MsgQueue received_queue;
    Receiver receiver(received_queue, is_running);
    MsgQueue to_send_queue;
    Transmitter transmitter(to_send_queue, is_running);
    AudioController audio_controller;
    std::thread outcoming_thread(outcoming_thread_function, std::ref(to_send_queue));
    for (;;) {
        Message received = received_queue.dequeue();
        switch(received.header){
            // case ASND:
            //     std::cout << received.numeric << std::endl;
            //     break;
            case TSND:
                std::cout << received.numeric << std::endl;
                break;
            case ASET:
                audio_controller.setLevel(received.numeric);
                break;
            case KEYP:
                //keyctrl
                break;
            case HGET:
                //keyctrl
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
    is_running.store(false);
}