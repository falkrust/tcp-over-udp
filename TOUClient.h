#ifndef TOUCLIENT_H
#define TOUCLIENT_H

enum CLIENT_STATE {NOT_INIT, CLOSED, SYN_SENT, ESTABLISHED, FIN_WAIT_1,
					FIN_WAIT_2, TIME_WAIT};

class TOUClient {
private:
	char *domainName;
	int port;
	int sockfd;
	CLIENT_STATE currentState;
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
	 * return 0 on success, and nonzero otherwise
	 */
	int init();
	/*
	 * Connect to server
	 * return 0 on success, and nonzero otherwise
	 */
	int connect();
};

#endif