#include "tcpEpollServer.h"
#include "public.h"
#include "fileTransfer.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXEVENTS 1000

/*
 * 服务器端构造函数
 *
 * */
TcpEpollServer::TcpEpollServer(std::string& ip, unsigned short port) {
	signal(SIGCHLD, SIG_IGN);
	_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == _lfd) {
		perror("socket error:");
		exit(0);
	}

	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip.c_str());

	int res = bind(_lfd, (struct sockaddr*)&ser, sizeof(ser));
	if (-1 == res) {
		perror("bind error!");
		exit(0);
	}
	res = listen(_lfd, 5);
	if (-1 == res) {
		perror("listen error!");
		exit(0);
	}
	
	_epfd = epoll_create(5);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _lfd;

	epoll_ctl(_epfd, EPOLL_CTL_ADD, _lfd, &ev);
	_fileTransfer = new FileTransfer();
}


TcpEpollServer::~TcpEpollServer() {
	delete _fileTransfer;
}



void TcpEpollServer::start() {
	while (1) {
		struct epoll_event evs[MAXEVENTS];
		int n = epoll_wait(_epfd, evs, MAXEVENTS, -1);
		if (0 == n || -1 == n) {
			std::cout << "epoll_wait error!" << std::endl;
			break;
		}

		for (int i=0; i<n; ++i) {
			int fd = evs[i].data.fd;
			if (_lfd == fd) {
				getClient();
				continue;
			} else if (evs[i].events & EPOLLRDHUP) {
				removeClientLink(fd);
				continue;
			} else if (evs[i].events & EPOLLIN) {
				handlerClientData(fd);
			}
		}
	}
}



void TcpEpollServer::getClient() {
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	int cfd = accept(_lfd, (struct sockaddr*)&cli, &len);
	if (-1 == cfd) {
		perror("accept error:");
		exit(0);
	}

	char ipbuf[64] = {0};
	std::cout << "client linked: " << cfd << std::endl;
	std::cout << "ip address:" << inet_ntop(AF_INET, &cli.sin_addr.s_addr, ipbuf, sizeof(ipbuf)) << std::endl;
	std::cout << "port:" << ntohs(cli.sin_port) << std::endl;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.fd = cfd;
	epoll_ctl(_epfd, EPOLL_CTL_ADD, cfd, &ev);
}

void TcpEpollServer::removeClientLink(int cfd) {
	close(cfd);
	epoll_ctl(_epfd, EPOLL_CTL_DEL, cfd, NULL);
}



/*
 * epoll服务器处理客户端发送过来的数据
 *
 * */
void TcpEpollServer::handlerClientData(int _socketfd) {
	msg_t msg;
	bzero(&msg, sizeof(msg));

	int n = recv(_socketfd, &msg, sizeof(msg), 0);
	if (n <= 0) {
		removeClientLink(_socketfd); // 客户端断开连接
		return;
	}
	
	char *p = NULL;
	std::string str;
	switch (msg.type) {
		case UPLOAD:
			p = strtok(msg.text, " ");
			p = strtok(NULL, " ");
			send(_socketfd, "OK", 2, 0);
			_fileTransfer->download(std::move(std::string(p)), _socketfd, 0);
			break;
		case DOWNLOAD:
			p = strtok(msg.text, " ");
			p = strtok(NULL, " ");
			_fileTransfer->upload(std::move(std::string(p)), _socketfd, 0);
			break;
		case COMMAND:
			str = msg.text;
			handlerClientCmd(_socketfd, str);
			break;
		default:
			std::cout << "error!" << std::endl;
			break;
	}
}

void TcpEpollServer::handlerClientCmd(int fd, std::string& cmd) {
	std::cout << cmd << std::endl;
	char* args[128] = {0};
	int  pos = 0;
	char *p = strtok((char*)cmd.c_str(), " ");
	while (p) {
		args[pos ++] = p;
		p = strtok(NULL, " ");
	}

	if (!strncmp("cd", cmd.c_str(), 2)) {
		chdir(args[1]);
		send(fd, "chdir success!\n", strlen("chdir success!\n"), 0);
		return;
	}

	int fds[2];
	pipe(fds);
	pid_t pid = fork();
	if (0 == pid){
		close(fds[0]);
		close(1);
		close(2);
		dup(fds[1]);
		dup(fds[1]);
	
		char path[128] = "/bin/";
		strcat(path, args[0]);
		execv(path, args);
		write(fds[1], "cmd not found!\n", strlen("cmd not found!\n"));
	} else {
		close(fds[1]);
		while (1){
			char buf[1024] = {0};
			int n = read(fds[0], buf, 1023);
			if (n <= 0) {
				strcpy(buf, "cmd sucecess!\n");
				send(fd, buf, strlen(buf), 0);
				break;
			} else {
				send(fd, buf, strlen(buf), 0);
			}
		}
	}
}
