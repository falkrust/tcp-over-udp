#ifndef TCPOVERUDP_H
#define TCPOVERUDP_H

#include "TOUClient.h"
#include "TOUServer.h"

class TCPOverUDP {
private:

public:
	static TOUClient getClient(char *domainName, int port);
    static TOUServer getServer(int port);
};

#endif  // TCPOVERUDP_H
