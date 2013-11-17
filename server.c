#include "TCPOverUDP.h"
#include "TOUSegment.h"
#include <stdio.h>

int main(int argc, char ** argv) {
	int port = 8080;
	TOUServer serv(port);
	printf("Server was created successfully\n");

	serv.init();
	serv.listen();

	char * buf = new char[10001];

	serv.recv(buf, 10000);

	/*
	TOUSegment synSegment(1, 2, 3, false, false, true);
	char buffer[13];
	synSegment.putHeader(buffer);
	buffer[12] = '\0';
	TOUSegment segment = TOUSegment::parseSegment(buffer, 12);
	printf("isACK=%d isFIN=%d isSYN=%d SEQ=%d ACK=%d\n", segment.isACK(),
					segment.isFin(), segment.isSyn(),
					segment.getSequenceNum(), 
					segment.getACKNum());

	TOUSegment::putInt(buffer, 123);
	printf("Read %lu\n", TOUSegment::readInt(buffer));
	*/
	return 0;
}
