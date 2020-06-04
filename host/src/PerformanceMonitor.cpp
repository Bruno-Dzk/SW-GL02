#define _GLIBCXX_USE_CXX11_ABI 0
#include "PerformanceMonitor.hpp"
#include <stdio.h>
#include <iostream>
#include <string>   
using namespace std;

/**
 * Returns output of given command as a string
 */
std::string PerformanceMonitor::GetStdoutFromCommand(std::string cmd)
{
    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(cmd.c_str(), "r");
    if(stream)
    {
        while (!feof(stream))
        {
            if(fgets(buffer,max_buffer,stream) != NULL) data.append(buffer);
                pclose(stream);
        }
        
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
    string cmd = "mpstat | awk '$12 ~ /[0-9.]+/ { print 100 - $12 }'";
    string sCpu = this->GetStdoutFromCommand(cmd);
    double dCpu = stod(sCpu);
    cout<< static_cast<int>(dCpu+0.5)<<endl;
    return 0;
}
/**
 * Checks ram usage using GetStdoutFromCommand()
 * convert output to int and returns it
 */
int PerformanceMonitor::getRAMUsage()
{
    string cmd = "free | grep Mem | awk '{print $3/$2 * 100.0}'";
    string sRam = this->GetStdoutFromCommand(cmd);
    double dRam = stod(sRam);
    cout<< static_cast<int>(dRam+0.5)<<endl;
    return 0;
}
/**
 * Checks cpu tem using GetStdoutFromCommand()
 * convert output to int and returns it
 * needs lm-sensors
 */
int PerformanceMonitor::getCPUTemp()
{
    string cmd = "sensors -u | grep -A 1 'Package id 0:' | sed '1d' | awk '{print $2}'";
    string stemp = this->GetStdoutFromCommand(cmd);
    double dtemp = stod(stemp);
    cout<<static_cast<int>(dtemp+0.5)<<endl;
    return 0;
}