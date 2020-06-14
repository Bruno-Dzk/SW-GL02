#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
class PerformanceMonitor
{
private:
    // returns output of a command
    std::string getStdoutFromCommand(std::string cmd);
public:
    int getCPUUsage();
    int getRAMUsage();
    int getCPUTemp();
};

