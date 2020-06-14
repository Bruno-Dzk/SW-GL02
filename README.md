# SW-GL02
A simple system connecting a PC to an external FreeRTOS microcontroller through UART. Done for an university project. 

**Library installation**:
sudo apt-get install libasound2-dev libasound2

To compile add "-lasound -lpthread" options

For PerformanceMonitor:
    sudo apt-get install lm-sensors
    sudo apt install sysstat

For VideoColorMonitor, if not pre-installed:
    sudo apt-get install imagemagick
