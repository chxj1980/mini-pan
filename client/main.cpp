#include "client.h"
#include <cstdlib>


int main(int argc, char *argv[]) {
	std::string ip = "127.0.0.1";
	Client cli(ip, atoi(argv[1]));
	cli.start();
	return 0;
}
