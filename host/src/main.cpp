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

void outcoming_thread_function(MsgQueue & to_send_queue, std::atomic<bool>& arduino_running, AudioController & audio_controller){
    PerformanceMonitor performance_monitor;
    VideoColorMonitor video_cm;
    auto t1 = std::chrono::high_resolution_clock::now();
    double handshake_acc = 0.0;
    double perfmon_acc = 0.0;
    double audio_acc = 0.0;
    double vcolor_acc = 0.0;
    for (;;) {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> delta = t2 - t1;
        if(arduino_running.load()){
            // if(audio_acc > 200.0){
            //     Message audio_info(ASND, audio_controller.getLevel());
            //     to_send_queue.enqueue(audio_info);
            //     audio_acc = 0.0;
            // }
            if(perfmon_acc > 400.0){
                Message cpu_info(CSND, performance_monitor.getCPUUsage());
                to_send_queue.enqueue(cpu_info);
                Message ram_info(RSND, performance_monitor.getRAMUsage());
                to_send_queue.enqueue(ram_info);
                Message temp_info(TSND, performance_monitor.getCPUTemp());
                to_send_queue.enqueue(temp_info);
                perfmon_acc = 0.0;
            }

            // if(vcolor_acc > 100){
            //     unsigned int color = video_cm.getColor();
            //     // std::cout << color << std::endl;
            //     Message video_info(VSND, color);
            //     to_send_queue.enqueue(video_info);
            //     vcolor_acc = 0.0;
            // }

            perfmon_acc += delta.count();
            audio_acc += delta.count();
            vcolor_acc += delta.count();
        }
       
        if(handshake_acc > 3000.0){
            Message test = Message(HRDY, 69);
            to_send_queue.enqueue(test);
            handshake_acc = 0.0;
        }
        handshake_acc += delta.count();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        t1 = t2;
	}
}

int main(){
    std::atomic<bool> is_running(true);
    std::atomic<bool> arduino_running(false); //tt
    MsgQueue received_queue;
    Receiver receiver(received_queue, is_running);
    MsgQueue to_send_queue;
    Codec codec;
    Transmitter transmitter(to_send_queue, is_running);
    AudioController audio_controller;
    std::thread outcoming_thread(outcoming_thread_function, std::ref(to_send_queue), std::ref(arduino_running), std::ref(audio_controller));
    for (;;) {
        Message received = received_queue.dequeue();
        switch(received.header){
            // case ASND:
            //     std::cout << received.numeric << std::endl;
            //     break;
            case ERDY:
                if(!arduino_running.load()){
                    arduino_running.store(true);
                }
                std::cout << "YOOOOOO" <<std::endl;
                break;
            case ASET:
                std::cout << "a RCV: " << received.numeric << std::endl;
                audio_controller.setLevel(received.numeric);
                break;
            // case TSND:
            //     std::cout << "TGOT: " << received.numeric << std::endl;
            //     break;
            // case CSND:
            //     std::cout << "CGOT: " << received.numeric << std::endl;
            //     break;
            // case RSND:
            //     std::cout << "RGOT: " << received.numeric << std::endl;
            //     break;
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