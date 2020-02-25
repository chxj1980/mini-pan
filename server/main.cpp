#include "tcpEpollServer.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
	std::string ip = "127.0.0.1";
	
	TcpEpollServer ser(ip, atoi(argv[1]));

	ser.start();
	return 0;
}

