#define _GLIBCXX_USE_CXX11_ABI 0
#include "PerformanceMonitor.hpp"
#include <stdio.h>
#include <iostream>
#include <string>

/**
 * Returns output of given command as a string
 */
std::string PerformanceMonitor::getStdoutFromCommand(std::string cmd)
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(cmd.c_str(), "r");
    if(stream)
    {
        if(fgets(buffer,max_buffer,stream) != NULL) data.append(buffer);
            pclose(stream);
        
    }
    return data;
}
/**
 * Checks cpu usage using GetStdoutFromCommand()
 * convert output to int and returns it
 * needs mpstat installed
 */
int PerformanceMonitor::getCPUUsage()
{
    std::string cmd = "mpstat | egrep -v '^Linux|^$' |  awk -v c='%idle' '{print $NF}' | sed '1d' | awk '{print 100 - $1}'";
    std::string sCpu = this->getStdoutFromCommand(cmd);
    return stoi(sCpu);
    //double dCpu = std::stod(sCpu); wersja z zaokraglaniem
    //return static_cast<int>(dCpu+0.5);
}
/**
 * Checks ram usage using GetStdoutFromCommand()
 * convert output to int and returns it
 */
int PerformanceMonitor::getRAMUsage()
{
    std::string cmd = "free | grep Mem | awk '{print $3/$2 * 100.0}'";
    std::string sRam = this->getStdoutFromCommand(cmd);
    return stoi(sRam);
    //double dRam = stod(sRam); wersja z zaokraglaniem
    //return static_cast<int>(dRam+0.5);
}
/**
 * Checks cpu tem using GetStdoutFromCommand()
 * convert output to int and returns it
 * needs lm-sensors
 */
int PerformanceMonitor::getCPUTemp()
{
    std::string cmd = "sensors | grep 'Package id 0:' | awk '{print $4}'";
    std::string sTemp = this->getStdoutFromCommand(cmd);
    return stoi(sTemp);
    //double dTemp = stod(sTemp); wersja z zaokraglaniem
    //return static_cast<int>(dTemp+0.5);
}