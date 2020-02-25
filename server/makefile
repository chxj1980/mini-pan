main:fileTransfer.o tcpEpollServer.o main.o
	g++ fileTransfer.o tcpEpollServer.o main.o -o main -g
main.o:main.cpp
	g++ main.cpp -c
fileTransfer.o:fileTransfer.cpp
	g++ fileTransfer.cpp -c
tcpEpollServer.o:tcpEpollServer.cpp
	g++ tcpEpollServer.cpp -c

clean:
	rm -rf *.o
	rm main
