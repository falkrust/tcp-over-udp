#ifndef TOUSERVER_H
#define TOUSERVER_H
#include <netdb.h>

enum ServerState {SV_NOINIT, SV_CLOSED, SV_LISTEN, SV_SYNRCVD, SV_ESTABLISHED, SV_CLOSEWAIT,
					SV_LASTACK};

class TOUServer {
private:
	int port;
	int sockfd;
	addrinfo ainfo;
	ServerState currentState;
	void accept();
	sockaddr_storage dest;
	socklen_t addr_len;
	unsigned lastSeqRcvd;
	bool sendAck(unsigned);
public:
	TOUServer(int port);
	~TOUServer();
	bool init();
	bool listen();
	int recv(char * buf, int len);
	void finalizeConnection();
};

#endif	//TOUSERVER_H
