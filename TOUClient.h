#ifndef TOUCLIENT_H
#define TOUCLIENT_H

#include <netdb.h>
#include "TOUQueue.h"
#include <pthread.h>
enum CLIENT_STATE {CL_NOINIT, CL_CLOSED, CL_SYNSENT, CL_ESTABLISHED, CL_FINWAIT1,
					CL_FINWAIT2, CL_TIMEWAIT};

enum CLIENT_CNGSTNCTRL { SLOW_START, CONG_AVOIDANCE, FAST_RECOVERY};

class TOUClient {
private:
	int port;
	int sockfd;
	int ssthresh, cwnd, dupACKcount;
	int currentTimeout;	// in seconds
	int seqoffset;
	CLIENT_STATE currentState;
	CLIENT_CNGSTNCTRL congControlState;
	char * domainName;
	char * data;
	sockaddr_storage dest;
	unsigned int addr_len;
	unsigned int lowestSent, highestSent, highestACKd;
	unsigned lastSeqReceived;
	int toSend, hasSent;
	TOUQueue queue;
	pthread_t tid, sid;
	/* s_mutex - on socket, v_mutex - on different parameters */
	pthread_mutex_t s_mutex, v_mutex, q_mutex;
	static void * clientTimeoutWorker(void * clientPtr);
	static void * clientSendWorker(void * clientPtr);
	int sendSegment(int startIndex, int numbytestosend);
	void slowStartHandler(bool, unsigned);
	void congAvoidanceHandler(bool, unsigned);
	void fastRecoveryHandler(bool, unsigned);

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
