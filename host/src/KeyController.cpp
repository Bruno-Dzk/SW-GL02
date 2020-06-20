#include "KeyController.hpp"

void KeyController::keyPress(int key){
    /*
    Runs the function on the host.
    */
    switch (key)
        {
        case 0:
            system("nano file.txt");
            break;
        case 1:
            system("cal");
            break;
        case 2:
            system("sl");
            break;
        case 3:
            system("ls");
            break;
        case 4:
            system("pwd");
            break;
        case 5:
            system("tree ./");
            break;
        case 6:
            system("hostname");
            break;
        case 7:
            system("ps aux");
            break;
        default:
            break;
        }
}

std::string KeyController::getHelp(int key){
    /*
    Returns a description of the function.
    */
    return key_help[key];
}
