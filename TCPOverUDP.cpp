#include "TCPOverUDP.h"
#include "TOUClient.h"
#include <stdio.h>


TOUClient TCPOverUDP::getClient(char *domainName, int port) {
	printf("And here\n");

	#ifdef TOU_DEBUG
	printf("THIS IS DEBUG MODE\n");
	#endif

	return TOUClient(domainName, port);
}

TOUServer TCPOverUDP::getServer(int port) {
	return TOUServer(port);
}