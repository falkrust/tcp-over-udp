#include "TCPOverUDP.h"
#include "TOUClient.h"
#include <stdio.h>

void TCPOverUDP::getNextState() {
	printf("Hello world from here\n");
}

TOUClient TCPOverUDP::initClient() {
	TOUClient test;
	printf("And here\n");
	return test;	
}