CC=g++
CFLAGS=-Wall -g
main: main.cpp toulib.o
	$(CC) $(CFLAGS) -o test main.cpp toulib.o server.o client.o
toulib.o: TCPOverUDP.cpp TCPOverUDP.h server.o client.o
	$(CC) $(CFLAGS) -o toulib.o -c TCPOverUDP.cpp
server.o: TOUServer.cpp TOUServer.h
	$(CC) $(CFLAGS) -o server.o -c TOUServer.cpp 
client.o: TOUClient.cpp TOUClient.h
	$(CC) $(CFLAGS) -o client.o -c TOUClient.cpp 
clean:
	rm -f *.o
