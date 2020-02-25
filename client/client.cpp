#include "client.h"
#include "public.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

Client::Client(std::string& ip, unsigned short port) {
	_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == _socketfd) {
		perror("socket error:");
		exit(0);
	}

	struct sockaddr_in ser;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip.c_str());

	int res = connect(_socketfd, (struct sockaddr*)&ser, sizeof(ser));
	if (-1 == res) {
		perror("connect error:");
		exit(0);
	}

	_fileTransfer = new FileTransfer();
}	

Client::~Client() {
	delete _fileTransfer;
	close(_socketfd);
}

void Client::start() {
	while (1) {
		menu();
		char cmd[128] = {0};
		fgets(cmd, 128, stdin);
		cmd[strlen(cmd)-1] = 0;
		if (!strlen(cmd)) continue;
		if (!strncmp(cmd, "quit", 4)) exit(0);
		msg_t msg;
		bzero(&msg, sizeof(msg));
		strcpy(msg.text, cmd);
		if (!strncmp(cmd, "upload", 6)) {
			msg.type = UPLOAD;
			send(_socketfd, &msg, sizeof(msg.type)+strlen(msg.text), 0);
			char buf[128] = {0};
			recv(_socketfd, buf, 127, 0);
			std::cout << "buf:" << buf << std::endl;
			char *p = strtok(msg.text, " ");
			p = strtok(NULL, " ");
			std::cout << "p = " << p << std::endl;
			_fileTransfer->upload(std::move(std::string(p)), _socketfd, 1);
		} else if (!strncmp(cmd, "download", 8)) {
			msg.type = DOWNLOAD;
			send(_socketfd, &msg, sizeof(msg.type)+strlen(msg.text), 0);
			char *p = strtok(msg.text, " ");
			p = strtok(NULL, " ");
			_fileTransfer->download(std::move(std::string(p)), _socketfd, 1);
		}	else {
			msg.type = COMMAND;
			if (!strncmp(cmd, "ls", 2)) {
					strcat(msg.text, " -l");
			}
			send(_socketfd, &msg, sizeof(msg.type)+strlen(msg.text), 0);
			while (1) {
				char buf[1024] = {0};
				int n = recv(_socketfd, buf, 1023, 0);
				if (n <= 0) {
					exit(0);
				}
				std::cout << "buf:" << buf << std::endl;
				if (strstr(buf, "cmd success!")) break;
			}
		}
	}
}


void Client::menu() {
	std::cout << "================== *mini-pan* ================" << std::endl;
	std::cout << "================== *ls*       ================" << std::endl;
	std::cout << "================== *cd*       ================" << std::endl;
	std::cout << "================== *pwd* 	    ================" << std::endl;
	std::cout << "================== *mkdir* 		================" << std::endl;
	std::cout << "================== *download* ================" << std::endl;
	std::cout << "================== *upload*   ================" << std::endl;
	std::cout << "==============================================" << std::endl;
	std::cout << "please select: ";	
}
