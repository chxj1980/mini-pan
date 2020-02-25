main:main.o fileTransfer.o client.o 
	g++ main.o fileTransfer.o client.o -o main -g
main.o:main.cpp
	g++ main.cpp -c -g
client.o:client.cpp
	g++ client.cpp -c -g
fileTransfer.o:fileTransfer.cpp
	g++ fileTransfer.cpp -c -g

clean:
	rm -rf *.o
	rm main
