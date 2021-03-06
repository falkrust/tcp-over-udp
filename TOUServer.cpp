#include "TOUServer.h"
#include "TOUSegment.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_BUFLEN 2500


TOUServer::TOUServer(int port) {
	this->port = port;
	sockfd = -1;
	currentState = SV_NOINIT;
}

TOUServer::~TOUServer() {
	if (sockfd) {
		close(sockfd);
	}
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool TOUServer::init() {
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	const char * strPort = std::to_string((long long int)this->port).c_str();

	if ((rv = getaddrinfo(NULL, strPort, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return false;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("init(): failed to bind");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "init(): failed to bind socket\n");
		return false;
	}

	freeaddrinfo(servinfo);

	return true;
}

bool TOUServer::listen() {
	addr_len = sizeof dest;
	char s[INET6_ADDRSTRLEN];
	char buf[MAX_BUFLEN+1];
	int numbytes;

	if ((numbytes = recvfrom(sockfd, buf, MAX_BUFLEN-1 , 0,
		(struct sockaddr *)&dest, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
	}


	printf("listener: got packet from %s\n",
		inet_ntop(dest.ss_family,
			get_in_addr((struct sockaddr *)&dest),
			s, sizeof s));
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buf);

	TOUSegment syn = TOUSegment::parseSegment(buf, numbytes);

	/*
	printf("isACK=%d isFIN=%d isSYN=%d SEQ=%d ACK=%d\n", segment.isACK(),
					segment.isFin(), segment.isSyn(),
					segment.getSequenceNum(), 
					segment.getACKNum());
					*/
	if (!syn.isSyn() || syn.isAck() || syn.isFin()) {
		perror("listen(): incorrect segment received");
		return false;
	}
	// respond with synack packet
	TOUSegment synack(0, syn.getSequenceNum()+1, 2500, false, true, true);
	synack.putHeader(buf);
	if(sendto(sockfd, buf, TOU_HEADER_SIZE, 0, (struct sockaddr *)&dest, addr_len) == -1) {
		perror("listen(): failed to send synack segment");
		return false;
	}

	if ((numbytes = recvfrom(sockfd, buf, MAX_BUFLEN-1 , 0,
		(struct sockaddr *)&dest, &addr_len)) == -1) {
			perror("listen(): failed to get ack");
			return false;
	}

	TOUSegment ack = TOUSegment::parseSegment(buf, numbytes);
	if(ack.isSyn() || !ack.isAck() || ack.isFin()) {
		perror("listen(): failed to get ack");
		return false;
	}

	lastSeqRcvd = ack.getSequenceNum();
	// connection established
	printf("listen(): connection established\n");
	currentState = SV_ESTABLISHED;

	close(sockfd);
	return true;
}

int TOUServer::recv(char * buf, int len) {

	if (currentState != SV_ESTABLISHED) {
		perror("socket is not connected");
		return -1;
	}

	char buffer[2501];
	char *currentPtr= buf;
	int bytesLeft = len;
	int currentSize = 2500;
	int bytesRecvd = 0;
	int numbytes;
	while(true) {
		if ((numbytes = recvfrom(sockfd, buffer, currentSize-1 , 0,
			(struct sockaddr *)&dest, &addr_len)) == -1) {
				perror("listen(): failed to get ack");
				return -1;
		}

		TOUSegment seg = TOUSegment::parseSegment(buf, numbytes);
		if (seg.isFin()) {
			finalizeConnection();
			return bytesRecvd;	
		} else {

			int dataLen = numbytes - TOU_HEADER_SIZE;	// header size is 12 bytes
			if (dataLen > 0) {
				if (dataLen <= bytesLeft) {
					strncpy(currentPtr, buf, dataLen);
					currentPtr = &currentPtr[dataLen];
					bytesRecvd += dataLen;
				} else {
					return bytesRecvd;
				}	
			}
			unsigned seqNum = seg.getSequenceNum();
			sendAck(seqNum);
		}
	}
	printf("Inside accpet()\n");
}

void TOUServer::finalizeConnection() {
	// TODO:
}

bool TOUServer::sendAck(unsigned seqNum) {
	if (seqNum <= lastSeqRcvd) {
		// duplicate segment
		return true;
	} else {
		return true;
	}
}
