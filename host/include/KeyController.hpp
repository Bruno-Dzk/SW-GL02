#pragma once
#include <iostream>
#include <stdio.h>
#include <string>

class KeyController
{
public:
    void keyPress(int key);
    std::string getHelp(int key);

private:
    std::string key_help[8] = 
    {
    "Open new file in nano",
    "Shows calendar",
    "Shows train",
    "Shows files in directory",
    "Shows directory",
    "Shows tree of directory",
    "Shows host name",
    "Shows all proces"
    };
};
