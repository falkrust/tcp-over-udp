CC=g++
CFLAGS=-Wall -std=c++11 -g -DTOU_DEBUG
all: main toulib.o server.o client.o
main: main.cpp toulib.o server.o client.o
	$(CC) $(CFLAGS) -o main main.cpp toulib.o server.o client.o
toulib.o: TCPOverUDP.cpp TCPOverUDP.h
	$(CC) $(CFLAGS) -o toulib.o -c TCPOverUDP.cpp
server.o: TOUServer.cpp TOUServer.h
	$(CC) $(CFLAGS) -o server.o -c TOUServer.cpp 
client.o: TOUClient.cpp TOUClient.h
	$(CC) $(CFLAGS) -o client.o -c TOUClient.cpp 
clean:
	rm -f *.o main
