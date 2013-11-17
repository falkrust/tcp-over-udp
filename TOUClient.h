#ifndef TOUCLIENT_H
#define TOUCLIENT_H

#include <netdb.h>
enum CLIENT_STATE {CL_NOINIT, CL_CLOSED, CL_SYNSENT, CL_ESTABLISHED, CL_FINWAIT1,
					CL_FINWAIT2, CL_TIMEWAIT};

class TOUClient {
private:
	int port;
	int sockfd;
	CLIENT_STATE currentState;
	addrinfo ainfo;
	char *domainName;
public:
	/**
	 * Constructor
	 * domainName: domain name to which client will connect
	 * port: port number to which to connect
	 */
	TOUClient(char *domainName, int port);
	~TOUClient();

	/*
	 * Initialize the client
	 * return true on success, and false otherwise
	 */
	bool init();
	/*
	 * Connect to server
	 * return true on success, and false otherwise
	 */
	bool connect();
	bool send(char * data, int len);
};

#endif
