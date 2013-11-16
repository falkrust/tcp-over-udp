CC=g++
CFLAGS=-Wall -std=c++0x -g -DTOU_DEBUG
all: main toulib.o server.o client.o segment.o
main: main.cpp toulib.o server.o client.o segment.o
	$(CC) $(CFLAGS) -o main main.cpp toulib.o server.o client.o segment.o
toulib.o: TCPOverUDP.cpp TCPOverUDP.h
	$(CC) $(CFLAGS) -o toulib.o -c TCPOverUDP.cpp
server.o: TOUServer.cpp TOUServer.h
	$(CC) $(CFLAGS) -o server.o -c TOUServer.cpp 
client.o: TOUClient.cpp TOUClient.h
	$(CC) $(CFLAGS) -o client.o -c TOUClient.cpp 
segment.o: TOUSegment.cpp TOUSegment.h
	$(CC) $(CFLAGS) -o segment.o -c TOUSegment.cpp 
clean:
	rm -f *.o main
