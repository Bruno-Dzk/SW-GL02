#include "Transmitter.hpp"

Transmitter::Transmitter(MsgQueue &msgQueue, std::atomic<bool>& program_is_running)
{
	this->toSendQueue = &msgQueue;

	this->transmitter_thread = std::thread(&Transmitter::send, this);

	// saving the passed controlling variable
	// if it is true - the main program is still running
	// if it is false - the thread should be ended
	// all the changes provided to this variable will be visible in this 
	// thread as well.
	// the only condition, is that these changes SHOULD BE PROVIDED in 
	// the main function
	this->program_status = &program_is_running;
}

	void Transmitter::send()

	{
		// Codec class instance for encoding messages
		Codec codec;

		std::string str_message = "";

		int bits_written = 0;

		// open serial port in write_only mode 
		this->serial_port_fd = open("/dev/ttyACM0", O_WRONLY);

		// if any error occured while opening the port - print a message and exit the program
		if (this->serial_port_fd < 0)
		{
			std::cout << "Error : " << errno << "occured while attempting to open a serial port" << std::endl;
			std::cout << "Error string : " << strerror(errno) << std::endl;
			exit(-1);
		}

		struct termios tty;
		memset(&tty, 0, sizeof tty);

		// get already set settings 
		if (tcgetattr(this->serial_port_fd, &tty) != 0)
		{
			std::cout << "Error : " << errno << "occured while attempting to get port attr" << std::endl;
			std::cout << "Error string :" << strerror(errno) << std::endl;
		}

		tty.c_cflag &= ~PARENB; // disabling parity 
		tty.c_cflag &= ~CSTOPB; //  only one stop bit used in communication
		tty.c_cflag |= CS8; // 8 bits per byte 
		tty.c_cflag &= ~CRTSCTS; // disable RTS/CTS hardware flow control 
		tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines 

		tty.c_lflag &= ~ICANON;
		tty.c_lflag &= ~ECHO; // disable echo
		tty.c_lflag &= ~ECHOE; // disable erasure
		tty.c_lflag &= ~ECHONL; // disable new-line echo
		tty.c_lflag &= ~ISIG; // disable interpretation of INTR, QUIT and SUSP
		tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
		tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // disable any special handling of received bytes

		tty.c_oflag &= ~OPOST; // no special interpretation of output bytes
		tty.c_oflag &= ~ONLCR; // no conversion of newline to carriage return/line feed

		tty.c_cc[VTIME] = 0;    // set waiting time to 0 s
		tty.c_cc[VMIN] = 0;

		// set in/out baud rate to be 9600 respectively
		cfsetispeed(&tty, B9600);
		cfsetospeed(&tty, B9600);

		// save the setting, handle any possible errors
		if (tcsetattr(this->serial_port_fd, TCSANOW, &tty) != 0)
		{
			std::cout << "Error : " << errno << "occured while attempting to set port attr" << std::endl;
			std::cout << "Error string: " << strerror(errno) << std::endl;
		}

		// mian thread loop. 
		// recieve message from the toSendQueue, encode it and convert to a char array (table). Then send to a port

		// while the controlling variable is True - the loop runs
		// otherwise - it stops
		while(this->program_status->load())
		{	
			while(!toSendQueue->empty()){
				// get message from the queue
				Message mess = toSendQueue->dequeue();

				// encode it with a Codec class method
				str_message = codec.Encode(mess);

				// create a chartable of the suiting size
				char message_char_table[str_message.size()];

				// convert a string message to a char table
				for(int i = 0; i < str_message.size(); i++) {
					message_char_table[i] = str_message[i];
				}

				for(char c : message_char_table){
					std::cout << int(c) << " ";
				}
				std::cout << "\n";

				// attempt to send a message to a port
				// if it fails - print an error message
				if (write(this->serial_port_fd, message_char_table, sizeof(message_char_table)) < 0)
				{
					std::cout << "Error :" << errno << "occured while attempting to send message to a posrt" << std::endl;
					std::cout << "Error string :" << strerror(errno) << std::endl;
				}
				// char rc [1] = {char(189)};
				// if(write(this->serial_port_fd, rc, 1) < 0)
				// {
				// 	std::cout << "Error :" << errno << "occured while attempting to send message to a posrt" << std::endl;
				// 	std::cout << "Error string :" << strerror(errno) << std::endl;
				// }
				// make a thread sleep for a while
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}

	}

	Transmitter::~Transmitter()
	{
		// when an object of the class is destroyed - join the thread, if posssible
		if (this->transmitter_thread.joinable())
		{
			this->transmitter_thread.join();
		}

		// close the port
		close(this->serial_port_fd);
	}

