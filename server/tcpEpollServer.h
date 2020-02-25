#ifndef __TCP_SERVER__
#define __TCP_SERVER__
#include "fileTransfer.h"
#include <string>

class TcpEpollServer {
public:
	TcpEpollServer(std::string& ip, unsigned short port);
	~TcpEpollServer();
	void start();
private:
	void getClient();
	void handlerClientData(int socketfd);
	void handlerClientCmd(int socketfd, std::string& cmd);
	void removeClientLink(int socketfd);
private:
	FileTransfer* _fileTransfer;
	int _lfd;
	int _epfd;
};


#endif
