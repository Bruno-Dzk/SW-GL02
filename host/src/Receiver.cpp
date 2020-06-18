#include "Receiver.hpp"

Receiver::Receiver(MsgQueue &msgQueue, std::atomic<bool> &isRunning) {

    this->receivedQueue = &msgQueue;
    // open port
    this->serialPort = open("/dev/ttyACM0", O_RDONLY);
    // check for errors while opening port
    this->isRunning = &isRunning;
    if (this->serialPort < 0) {
        std::cout << "Error " << errno << " from open: " << strerror(errno) << std::endl;
        exit(-1);
    }
    this->receiver_thread = std::thread(&Receiver::receive, this);
}

Receiver::~Receiver() {

    this->receiver_thread.join();
    // close port
    close(this->serialPort);
}

void Receiver::receive() {

    // struct for setting up port
    struct termios tty;

    // check for errors while trying to get port attributes
    if (tcgetattr(this->serialPort, &tty) != 0) {
        std::cout << "Error " << errno << " from " << strerror(errno) << std::endl;
    }

    // setting up serial port attributes
    tty.c_cflag &= ~PARENB; // disable parity bit
    tty.c_cflag &= ~CSTOPB; // one stop bit used
    tty.c_cflag |= CS8; // 8 bits per byte
    tty.c_cflag |= CREAD | CLOCAL; // turn on READ and ignore ctr lines

    tty.c_lflag &= ~ICANON; // disable canonical mode
    tty.c_lflag &= ~ECHO; // disable echo
    tty.c_lflag &= ~ECHOE; // disable erasure echo
    tty.c_lflag &= ~ECHONL; // disable new line echo
    tty.c_lflag &= ~ISIG; // disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off software work flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // disable special handling of bytes

    tty.c_oflag &= ~OPOST; // prevent special interpretation of output
    tty.c_oflag &= ~ONLCR; // prevent conversion of newline to carriage

    tty.c_cc[VTIME] = 5;
    tty.c_cc[VMIN] = 0;

    // set baud rate at 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // save settings checking for error
    if (tcsetattr(this->serialPort, TCSANOW, &tty) != 0) {
        std::cout << "Error " << errno << " from tcsetattr " << strerror(errno) << std::endl;
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //required to make flush work, for some reason
    // tcflush(this->serialPort,TCIOFLUSH);

    // read data
    int bytes_read;
    unsigned char messageStart;
    char header[5];
    unsigned char numberOfBytes;
    char data[28];
    unsigned char controlSumRead;

    Codec decoder;
    while(isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        bytes_read = read(this->serialPort, &messageStart, 1);
        // if(bytes_read > 0){
        //     std::cout << "b: " << int(messageStart) << std::endl;
        // }
        do {
            bytes_read = read(this->serialPort, &messageStart, 1);
            if(bytes_read > 0){
                std::cout << "start: " << int(messageStart) << std::endl;
            }
        } while(messageStart != 254);

        // read header
        int read_for_header = 0;
        do {
            char byte;
            bytes_read = read(this->serialPort, &byte, 1);
            if(bytes_read == 0){
                // std::cout << "sthwrng" << std::endl;
            }
            header[read_for_header] = byte;
            read_for_header += bytes_read;
        } while(read_for_header < 4);
        header[5] = '\0';
        std::string headerString(header, header + 4);
        // std::cout << "HDR: " << header << std::endl;

        if(headerString == "HSND"){
            read(this->serialPort, &numberOfBytes, 1);
        }else{
            numberOfBytes = 5;
        }       
        // std::cout << "noc: " << (int)numberOfBytes << std::endl;
        int read_for_data = 0;
        do {
            char byte;
            bytes_read = read(this->serialPort, &byte, 1);
            if(bytes_read > 0){
                // std::cout << "b: " << int(byte) << std::endl;
            }
            data[read_for_data] = byte;
            read_for_data += bytes_read;
        } while(read_for_data < numberOfBytes);
        data[numberOfBytes] = '\0';
        read(this->serialPort, &controlSumRead, 1);
        std::cout << "cs: " << int(controlSumRead) << std::endl;
        
        // int size;
        // memset(&data, '\0', sizeof(data));
        // std::string headerString(header, header + sizeof(header));
        // //std::cout << headerString << std::endl;
        // if(headerString == "HSND") {
        //     // header HSND means that help for buttons will be received
        //     msg = read(this->serialPort, &numberOfBytes, 1);
        //     if(msg < 0) {
        //         std::cout << "Error while reading from port" << std::endl;
        //     }
        //     data[0] = numberOfBytes;
        //     msg = read(this->serialPort, &data[1], int(numberOfBytes) + 1);
        //     if(msg < 0) {
        //         std::cout << "Error while reading from port" << std::endl;
        //     }

        //     // something went wrong, start over
        //     if(msg != numberOfBytes + 1) {
        //         continue;
        //     }
        //     size = msg;

        // } else {
        //     // any other header means that 6 more bytes will be received
        //     msg = read(this->serialPort, &data, 6);
        //     if(msg < 0) {
        //         std::cout << "Error while reading from port" << std::endl;
        //     }

        //     // something went wrong, start over
        //     if(msg != 6) {
        //         continue;
        //     }
        //     size = msg - 1;
        // }

        // check if control sum is correct
        unsigned char controlSum = 0;
        unsigned char n;
        // count set bits in header
        for(unsigned char i : header) {
            n = i;
            while(n) {
                controlSum += n & 1;
                n >>= 1;
            }
        }

        // count set bits in data
        for(int i = 0; i < numberOfBytes; i++) {
            n = data[i];
            while(n) {
                controlSum += n & 1;
                n >>= 1;
            }
        }
        std::cout << "TCS: " << int(controlSum) << std::endl;

        // std::cout << "dupa: " << int(messageStart) << std::endl;

        // if control sum is correct decode message and push to queue
        if(controlSum == controlSumRead) {
            std::string stringData(reinterpret_cast<char *>(data));
            std::string messageString = char(messageStart) + headerString + stringData;
            // for(char c : messageString){
            //     std::cout << int(c) << " ";
            // }
            // std::cout << std::endl;
            Message message = decoder.Decode(messageString);
            receivedQueue->enqueue(message);
        }
    }
}