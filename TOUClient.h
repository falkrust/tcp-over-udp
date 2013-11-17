#ifndef TOUCLIENT_H
#define TOUCLIENT_H

#include <netdb.h>
#include "TOUTimer.h"
#include <pthread.h>
enum CLIENT_STATE {CL_NOINIT, CL_CLOSED, CL_SYNSENT, CL_ESTABLISHED, CL_FINWAIT1,
					CL_FINWAIT2, CL_TIMEWAIT};

enum CLIENT_CNGSTNCTRL { SLOW_START, CONG_AVOIDANCE, FAST_RECOVERY};

class TOUClient {
private:
	int port;
	int sockfd;
	unsigned ssthresh, cwnd, dupACKcount;
	CLIENT_STATE currentState;
	CLIENT_CNGSTNCTRL congState;
	char *domainName;

	unsigned int lowestSent, highestSent, highestACKd;
	TOUTimer timer;
	pthread_t tid, sid;
	/* s_mutex - on socket, v_mutex - on different parameters */
	pthread_mutex_t s_mutex, v_mutex;
	static void * clientTimeoutWorker(void * clientPtr);
	static void * clientSendWorker(void * clientPtr);

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
