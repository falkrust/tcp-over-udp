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

#include "TOUClient.h"
#include "TOUSegment.h"

#define RECV_TIMEOUT 4

using namespace std;

TOUClient::TOUClient(char *domainName, int port) {
	this->domainName = domainName;
	this->port = port;
	this->currentState = CL_NOINIT;
	this->sockfd = -1;
}

TOUClient::~TOUClient() {
	// TODO
	printf("Inside destructor\n");
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
		tv.tv_sec = RECV_TIMEOUT;
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

		struct sockaddr_storage their_addr;
		unsigned int addr_len = sizeof their_addr;
		int numbytes;
		if ((numbytes = recvfrom(sockfd, buffer, TOU_HEADER_SIZE , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("connect(): failed to receive SYN_ACK");
			return false;
		}

		printf("Client received %d bytes\n", numbytes);
		char s[INET6_ADDRSTRLEN];
		printf("listener: got packet from %s\n",
		        inet_ntop(their_addr.ss_family,
		            get_in_addr_1((struct sockaddr *)&their_addr),
		            s, sizeof s));

		TOUSegment synack = TOUSegment::parseSegment(buffer, numbytes);
		if (!synack.isSyn() || !synack.isAck() || synack.isFin()) {
			perror("connect(): failed to receive synack");
			return false;
		}
		TOUSegment ack(0, 0, 2500, false, true, false);
		ack.putHeader(buffer);
		if (sendto(sockfd, buffer, TOU_HEADER_SIZE, 0, (struct sockaddr*)&their_addr, addr_len) == -1) {
			perror("Failed to send ack");
			return false;
		}
		printf("Connection established\n");
		return true;
	} else {
		perror("Failed to set header info");
		return false;
	}
}


bool TOUClient::send(char * data, int len) {
	return false;
}
