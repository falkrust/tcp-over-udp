CC=g++
CFLAGS=-Wall -std=c++0x -g -DTOU_DEBUG
all: toulib.o touserver.o touclient.o tousegment.o touqueue.o server client
toulib.o: TCPOverUDP.cpp TCPOverUDP.h
	$(CC) $(CFLAGS) -o toulib.o -c TCPOverUDP.cpp
touserver.o: TOUServer.cpp TOUServer.h
	$(CC) $(CFLAGS) -o touserver.o -c TOUServer.cpp 
touclient.o: TOUClient.cpp TOUClient.h
	$(CC) $(CFLAGS) -o touclient.o -c TOUClient.cpp 
tousegment.o: TOUSegment.cpp TOUSegment.h
	$(CC) $(CFLAGS) -o tousegment.o -c TOUSegment.cpp 
touqueue.o: TOUQueue.cpp TOUQueue.h
	$(CC) $(CFLAGS) -o touqueue.o -c TOUQueue.cpp 
client: touclient.o toulib.o tousegment.o client.c touqueue.o
	$(CC) $(CFLAGS) -o client client.c touclient.o touserver.o toulib.o tousegment.o touqueue.o -lpthread
server: touserver.o toulib.o tousegment.o server.c touqueue.o
	$(CC) $(CFLAGS) -o server server.c touserver.o touclient.o toulib.o tousegment.o touqueue.o -lpthread
clean:
	rm -f *.o server client
