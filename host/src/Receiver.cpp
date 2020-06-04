#include "Receiver.hpp"

Receiver::Receiver(MsgQueue &msgQueue) {
    this->receivedQueue = &msgQueue;
    // open port
    this->serialPort = open("/dev/pts/2", O_RDONLY);
    // check for errors while opening port
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

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 11; // wait for at least 11 bytes

    // set baud rate at 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // save settings checking for error
    if (tcsetattr(this->serialPort, TCSANOW, &tty) != 0) {
        std::cout << "Error " << errno << " from tcsetattr " << strerror(errno) << std::endl;
    }

    // read data
    int msg;
    unsigned char messageStart;
    unsigned char header[4];
    unsigned char numberOfCharacters;
    unsigned char data[28];

    Codec decoder;

    while(true) {
        // look for 255 to start message
        do {
            msg = read(this->serialPort, &messageStart, 1);
            if(msg < 0) {
                std::cout << "Error while reading from port" << std::endl;
            }
        } while(messageStart != 255);

        // read header
        msg = read(this->serialPort, &header, sizeof(header));
        if(msg < 0) {
            std::cout << "Error while reading from port" << std::endl;
        }

        int size;
        memset(&data, '\0', sizeof(data));
        std::string headerString(header, header + sizeof(header));
        if(headerString == "HSND") {
            // header HSND means that help for buttons will be received
            msg = read(this->serialPort, &numberOfCharacters, 1);
            if(msg < 0) {
                std::cout << "Error while reading from port" << std::endl;
            }
            data[0] = numberOfCharacters;
            msg = read(this->serialPort, &data[1], int(numberOfCharacters) + 1);
            if(msg < 0) {
                std::cout << "Error while reading from port" << std::endl;
            }

            // something went wrong, start over
            if(msg != numberOfCharacters + 1) {
                continue;
            }
            size = msg;

        } else {
            // any other header means that 6 more bytes will be received
            msg = read(this->serialPort, &data, 6);
            if(msg < 0) {
                std::cout << "Error while reading from port" << std::endl;
            }

            for(int i: data) {
                std::cout<<i<<std::endl;
            }

            // something went wrong, start over
            if(msg != 6) {
                continue;
            }
            size = msg - 1;
        }

        // check if control sum is correct
        unsigned char controlSum = 0;
        unsigned char controlSumRead = data[size];
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
        for(int i = 0; i < size; i++) {
            n = data[i];
            while(n) {
                controlSum += n & 1;
                n >>= 1;
            }
        }

        // if control sum is correct decode message and push to queue
        if(controlSum == controlSumRead) {
            std::string stringData(reinterpret_cast<char *>(data));
            std::string messageString = char(messageStart) + headerString + stringData;
            Message message = decoder.Decode(messageString);
            receivedQueue->enqueue(message);
        }
    }
}