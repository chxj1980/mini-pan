#include "fileTransfer.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>



void FileTransfer::upload(const std::string& filename, int socketfd, int flag) {
	int filefd = open(filename.c_str(), O_RDONLY);
	if (-1 == filefd) {
		send(socketfd, "file not found!\n", strlen("file not found!\n"), 0);
		return;
	}

	struct stat st;
	fstat(filefd, &st);
	char buf[128] = {0};
	sprintf(buf, "%ld", st.st_size);
	send(socketfd, buf, strlen(buf), 0); 

	bzero(buf, sizeof(buf));
	recv(socketfd, buf, 127, 0);
	if (!strncmp(buf, "ERROR", 5)) {
		close(filefd);
		return;
	}

	int sum = 0;
	while (1) {
		char msg[128] = {0};
		int n = read(socketfd, msg, 127);
		if (!n && flag) {
			std::cout << "finish upload! 100%" << std::endl;
			break;
		}
		sum += n;
		sleep(1);
		if (flag) {
			std::cout << "it has already finish: ";
			printf("%5.2f %%", sum * 100.0 / st.st_size);
			fflush(stdout);
			printf("\033[100D");
			printf("\033[K");
		}
		send(socketfd, msg, n, 0);
	}
	close(socketfd);
}


void FileTransfer::download(const std::string &filename, int socketfd, int flag) {
	char msg[128] = {0};
	int n = recv(socketfd, msg, 127, 0);
	if (!strcmp(msg, "file not found!\n")) return;
	
	int fileSize = 0;
	sscanf(msg, "%d", &fileSize); // 获取文件大小
	int fd = open(filename.c_str(), O_CREAT | O_WRONLY, 0664);
	if (-1 == fd) {
		send(socketfd, "ERROR", 5, 0);
		return;
	} else {
		send(socketfd, "OK", 2, 0);
	}

	int sum = 0;
	while (1) {
		char buf[128] = {0};
		int n = recv(socketfd, buf, 127, 0);
		if (n <= 0) break;
		write(fd, buf, n);
		sum += n;//	更新当前已经写入文件的字节数
		sleep(1);
		if (sum == fileSize && flag) {
			std::cout << "download finish: 100%" << std::endl;	
			break;
		}

		if (flag) {
			printf("download has already finish:");
			printf("%5.2f %%", sum * 100.0 / fileSize);
			fflush(stdout);
			printf("\033[100D");
			printf("\033[K");
		}
	}
	close(fd);
}















