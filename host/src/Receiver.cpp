#include "Receiver.hpp"

Receiver::Receiver(MsgQueue &msgQueue) {
    this->receivedQueue = &msgQueue;
    this->receiver_thread = std::thread(&Receiver::receive, this);
}

void Receiver::receive() {

    // open port
    int serialPort = open("/dev/pts/4", O_RDONLY);

    // struct for setting up port
    struct termios tty;

    // check for errors while opening port
    if (serialPort < 0) {
        std::cout << "Error " << errno << " from open: " << strerror(errno) << std::endl;
        exit(-1);
    }

    // check for errors while trying to get port attributes
    if (tcgetattr(serialPort, &tty) != 0) {
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

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 10; // wait for at least 10 bytes

    // set baud rate at 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // save settings checking for error
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        std::cout << "Error " << errno << " from tcsetattr " << strerror(errno) << std::endl;
    }

    // read data
    unsigned char readBuffer[34];
    memset(&readBuffer, '\0', sizeof(readBuffer));

    int msg = read(serialPort, &readBuffer, sizeof(readBuffer));
    if (msg < 0) {
        std::cout << "Error while reading from port" << std::endl;
    }

    // close port
    close(serialPort);

    // count set bits in last byte of message - to check control sum
    unsigned int controlSum = 0;
    unsigned int n = readBuffer[msg - 1];
    while (n) {
        controlSum += n & 1;
        n >>= 1;
    }

    // convert to string
    std::string mess = "";
    for (int i = 0; i < msg; i++) {
        mess += readBuffer[i];
    }

    // check if message is correct
    // number of bytes read between 11 and 34
    if (msg >= 11 && msg <= 34) {
        // check if first byte equals 255
        if ((int)readBuffer[0] == 255) {
            // check control sum
            if ((int)readBuffer[msg - 1] == controlSum) {
                // message correct, push to queue
                Message message = Codec.decode(mess);
                receivedQueue->enqueue(message);
            }
        }
    }
}