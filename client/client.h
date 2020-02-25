#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "fileTransfer.h"

class Client {
public:
	Client(std::string& ip, unsigned short port);
	~Client();
	void start();
private:
	void menu();

	int _socketfd;

	FileTransfer* _fileTransfer;
};


#endif
