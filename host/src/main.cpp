#include "AudioController.hpp"
#include "PerformanceMonitor.hpp"
#include "VideoColorMonitor.hpp"
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
    double vcolor_acc = 0.0;
    for (;;) {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> delta = t2 - t1;
        VideoColorMonitor video_cm;
        // if(audio_acc > 30){
        //     AudioController audio_controller;
        //     Message audio_info(ASND, audio_controller.getLevel());
        //     to_send_queue.enqueue(audio_info);
        //     audio_acc = 0.0;
        // }

        // if(vcolor_acc > 20){
        //     unsigned int color = video_cm.getColor();
        //     // std::cout << color << std::endl;
        //     Message video_info(VSND, color);
        //     to_send_queue.enqueue(video_info);
        //     vcolor_acc = 0.0;
        // }
       
        // if(perfmon_acc > 100.0){
        //     Message cpu_info(CSND, performance_monitor.getCPUUsage());
        //     to_send_queue.enqueue(cpu_info);
        //     Message ram_info(RSND, performance_monitor.getRAMUsage());
        //     to_send_queue.enqueue(ram_info);
        //     Message temp_info(TSND, performance_monitor.getCPUTemp());
        //     to_send_queue.enqueue(temp_info);
        //     Message testmes(ASET, 1024);
        //     to_send_queue.enqueue(testmes);
        //     perfmon_acc = 0.0;
        // }
        Message test = Message(HRDY, 69);
        to_send_queue.enqueue(test);

        Message test2 = Message(HSND, "EEEEEEE");
        to_send_queue.enqueue(test2);

        perfmon_acc += delta.count();
        audio_acc += delta.count();
        vcolor_acc += delta.count();
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        t1 = t2;
	}
}

int main(){
    std::atomic<bool> is_running(true);
    std::atomic<bool> arduino_running(true);
    MsgQueue received_queue;
    Receiver receiver(received_queue, is_running);
    MsgQueue to_send_queue;
    Codec codec;
    Transmitter transmitter(to_send_queue, is_running, arduino_running);
    AudioController audio_controller;
    std::thread outcoming_thread(outcoming_thread_function, std::ref(to_send_queue));
    for (;;) {
        Message received = received_queue.dequeue();
        switch(received.header){
            // case ASND:
            //     std::cout << received.numeric << std::endl;
            //     break;
            case HSND:
                std::cout << received.text << std::endl;
                break;
            case ASET:
                std::cout << "a RCV: " << received.numeric << std::endl;
                audio_controller.setLevel(received.numeric);
                break;
            case VSND:
                std::cout << "RCV: " << received.numeric << std::endl;
                break;
            case KEYP:
                //keyctrl
                break;
            case HGET:
                //keyctrl
                break;
        }
	}
    is_running.store(false);
}