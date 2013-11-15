#ifndef TCPOVERUDP_H
#define TCPOVERUDP_H

#include "TOUClient.h"
#include "TOUServer.h"

class TCPOverUDP {
private:
	int currentState;

public:
	void getNextState();
	TOUClient initClient();
    TOUServer initServer();
};

#endif  // TCPOVERUDP_H