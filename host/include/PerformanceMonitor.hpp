#include <string>
class PerformanceMonitor
{
private:
    // returns output of a command
    std::string GetStdoutFromCommand(std::string cmd);
public:
    int getCPUUsage();
    int getRAMUsage();
    int getCPUTemp();
};

