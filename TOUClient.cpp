#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <algorithm>

#include "TOUClient.h"
#include "TOUSegment.h"
#include "TOUQueue.h"
#include <signal.h>
#include <sys/time.h>

#define RECV_CONNECTING_TIMEOUT 4
#define MSS 100
#define DEFAULT_SSTHRESH 1000
#define DEFAULT_WORKER_SLEEP_PERIOD 5
#define RECEIVER_BUFFER_SIZE 2500
#define DEFAULT_CLIENT_TIMEOUT 5
#define SEGMENT_SIZE 100

using namespace std;

TOUClient::TOUClient(char *domainName, int port) {
	this->domainName = domainName;
	this->port = port;
	this->currentState = CL_NOINIT;
	this->sockfd = -1;
	this->ssthresh = 1000;
	pthread_mutex_init(&s_mutex, NULL);
	pthread_mutex_init(&v_mutex, NULL);
	pthread_mutex_init(&q_mutex, NULL);
	this->congControlState = SLOW_START;
	this->lastSeqReceived = 0;
	this->currentTimeout = DEFAULT_CLIENT_TIMEOUT;
	this->seqoffset = 2;
}

TOUClient::~TOUClient() {
	// TODO
	pthread_kill(tid, SIGINT);
	printf("Inside TOUClient destructor\n");
	pthread_mutex_destroy(&s_mutex);
	pthread_mutex_destroy(&v_mutex);
	pthread_mutex_destroy(&q_mutex);
}

void *get_in_addr_1(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**
 * Initialize the client
 */
bool TOUClient::init() {
	struct addrinfo hints, *servinfo, *p;
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	const char * strPort = to_string(this->port+1).c_str();

	if ((rv = getaddrinfo(NULL, strPort, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return false;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		// enable timeout on socket when waiting for data
		struct timeval tv;
		tv.tv_sec = RECV_CONNECTING_TIMEOUT;
		tv.tv_usec = 0;

		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			perror("connect(): setsockopt failed");
			return false;
		}

		// bind to socket b/c client will also be listening		
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
			perror("connect(): bind failed");
			return false;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to bind socket\n");
		return false;
	}
	freeaddrinfo(servinfo);
	this->currentState = CL_CLOSED;
	// return with no error
	return true;
}

bool TOUClient::connect() {
	if (this->currentState == CL_NOINIT) {
		perror("connect(): the client is not initialized");
		return false;
	} else if (this->currentState != CL_CLOSED) {
		perror("connect(): the client is not in CLOSED state");
		return false;
	}

	struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	int rv;
	const char * strPort = std::to_string(port).c_str();
	if ((rv = getaddrinfo(domainName, strPort, &hints, &servinfo)) != 0) {
		perror("connect(): failed to generate server address");
		return false;
	}



	char buffer[TOU_HEADER_SIZE + 1];
	TOUSegment synSegment(0, 0, 2500, false, false, true);
	if(synSegment.putHeader(buffer)) {
		if (sendto(sockfd, buffer, TOU_HEADER_SIZE, 0, (struct sockaddr*)servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
			perror("Failed to send data over socket");
			return false;
		}

		printf("The data was sent\n");
		unsigned int addr_len = sizeof dest;
		int numbytes;
		if ((numbytes = recvfrom(sockfd, buffer, TOU_HEADER_SIZE , 0,
			(struct sockaddr *)&dest, &addr_len)) == -1) {
			perror("connect(): failed to receive SYN_ACK");
			return false;
		}

		printf("Client received %d bytes\n", numbytes);
		char s[INET6_ADDRSTRLEN];
		printf("listener: got packet from %s\n",
				inet_ntop(dest.ss_family,
					get_in_addr_1((struct sockaddr *)&dest),
					s, sizeof s));

		TOUSegment synack = TOUSegment::parseSegment(buffer, numbytes);
		if (!synack.isSyn() || !synack.isAck() || synack.isFin()) {
			perror("connect(): failed to receive synack");
			return false;
		}
		lastSeqReceived = synack.getSequenceNum();
		TOUSegment ack(1, lastSeqReceived+1, 2500, false, true, false);
		ack.putHeader(buffer);
		if (sendto(sockfd, buffer, TOU_HEADER_SIZE, 0, (struct sockaddr*)&dest, addr_len) == -1) {
			perror("Failed to send ack");
			return false;
		}
		printf("Connection established\n");


		// launch thread for handling timeouts
		pthread_create(&tid, NULL, clientTimeoutWorker, this);
		printf("Created thread\n");

		currentState = CL_ESTABLISHED;


		return true;
	} else {
		perror("Failed to set header info");
		return false;
	}
}


bool TOUClient::send(char * data, int len) {
	if (data == NULL or len < 1) {
		perror("TOUClient::send(): invalid args");
		return false;
	}
	if (currentState != CL_ESTABLISHED) {
		perror("TOUClient::send(): socket is not connected");
		return false;
	}

	this->data = data;
	this->toSend = len;
	pthread_mutex_lock(&v_mutex);
	lowestSent = 0;
	highestSent = 0;
	highestACKd = 0;
	pthread_mutex_unlock(&v_mutex);

	pthread_t wid;
	pthread_create(&wid, NULL, clientSendWorker, this);

	// keep waiting for acks
	while(true) {

	}

	pthread_join(wid, NULL);
	return false;
}

void * TOUClient::clientTimeoutWorker(void * clientPtr) {
	TOUClient * cptr = (TOUClient*) clientPtr;
	TOUQueue queue = cptr->queue;	

	while(true) {
		long now = TOUQueue::getCurrentSeconds();
		list<QueueEntry> expired = queue.getExpired(now);
		if(expired.empty()) {
			QueueEntry front;
			if (queue.getFront(&front)) {
				sleep(front.dueTime - now);
			} else {
				sleep(DEFAULT_WORKER_SLEEP_PERIOD);
			}
		} else {
			// if this executes, then timeout happened
			for(list<QueueEntry>::iterator it=expired.begin(); it != expired.end(); ++it) {
				cptr->dupACKcount = 0;
				cptr->ssthresh = cptr->cwnd/2;
				switch (cptr->congControlState) {
					case SLOW_START:
								cptr->cwnd = cptr->ssthresh+3*MSS;
								break;
					case CONG_AVOIDANCE:
								cptr->cwnd = MSS;
								cptr->congControlState = SLOW_START;
								break;
					case FAST_RECOVERY:
								cptr->cwnd = 1;
								break;
				}

				QueueEntry cur = *it;
				cptr->sendSegment(cur.byteStart, cur.len);

			}
		}
	}
	return NULL;
}

void * TOUClient::clientSendWorker(void * clientPtr) {
	TOUClient * cptr = (TOUClient*) clientPtr;
	TOUQueue q = cptr->queue;
	int currentIndex = 0, toSend = cptr->toSend;
	while(true) {
		int unACKd = q.getLen();
		if((unACKd >= RECEIVER_BUFFER_SIZE) || (unACKd >= cptr->cwnd)) {
			sleep(DEFAULT_WORKER_SLEEP_PERIOD);
		} else {
			int bufsize = SEGMENT_SIZE < (toSend - currentIndex) ? SEGMENT_SIZE : (toSend - currentIndex);
			if (bufsize <= 0) {
				// all sent
				return NULL;
			}

			int sentBytes = cptr->sendSegment(currentIndex, bufsize);
			printf("Client has sent %d bytes\n", sentBytes);
		}
	}
	return NULL;
}

int TOUClient::sendSegment(int currentIndex, int numbytestosend) {
	TOUSegment segment(seqoffset+currentIndex, lastSeqReceived + 1,
			RECEIVER_BUFFER_SIZE, false, false, false);
	char buffer[SEGMENT_SIZE+TOU_HEADER_SIZE+1];
	segment.putHeader(buffer);
	strncpy(&buffer[TOU_HEADER_SIZE], &(data[currentIndex]), numbytestosend);
	unsigned int addr_len = sizeof dest;
	int sentThisTime;

	// socket access in critical region
	pthread_mutex_lock(&s_mutex);
	sentThisTime = sendto(sockfd, buffer, numbytestosend + TOU_HEADER_SIZE, 0,
		(struct sockaddr*)&dest, addr_len);
	pthread_mutex_unlock(&s_mutex);

	if (sentThisTime == -1) {
		perror("Failed to send data");
		return 0;
	} else {
		currentIndex += sentThisTime;
		if(sentThisTime == 0) {
			printf("Warning: 0 bytes sent\n");
		} else {
			queue.add(currentIndex + seqoffset, sentThisTime, currentTimeout);
		}
		return sentThisTime;
	}
}
