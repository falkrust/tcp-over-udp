#include "TOUClient.h"
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

using namespace std;

TOUClient::TOUClient(char *domainName, int port) {
	this->domainName = domainName;
	this->port = port;
	this->currentState = NOT_INIT;
}

TOUClient::~TOUClient() {
	// TODO
	printf("Inside destructor\n");
}
/**
 * Initialize the client
 */
int TOUClient::init() {
	struct addrinfo hints, *servinfo, *p;
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	const char * strPort = to_string(this->port).c_str();

	if ((rv = getaddrinfo(this->domainName, strPort, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to bind socket\n");
		return 2;
	}

	this->currentState = CLOSED;
	// return with no error
	return 0;
}

int TOUClient::connect() {
	if (this->currentState == NOT_INIT) {
		perror("connect(): the client is not initialized");
		return 1;
	} else if (this->currentState != CLOSED) {
		perror("connect(): the client is not in CLOSED state");
		return 1;
	}

	return 0;
}