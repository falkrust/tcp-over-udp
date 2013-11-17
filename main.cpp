#include "TCPOverUDP.h"
#include "TOUSegment.h"
#include <stdio.h>
#include <cstring>
#include <string>

using namespace std;

int main(int argc, char **argv) {
	char domainName[] = "localhost";
	int port = 8080;
	TOUClient client = TCPOverUDP::getClient(domainName, port);
	if (client.init() == 0) {
		printf("Successful client init\n");
	} else {
		printf("Failed to init client\n");
	}

	string content = "000023450112testdata";
	char buffer[1024];
	strcpy(buffer, content.c_str());
	TOUSegment::putInt(buffer, 165);
	TOUSegment segment = TOUSegment::parseSegment(buffer, 1024);
	printf("%d\n", segment.isACK());
	printf("size of long is %lu\n", sizeof (unsigned long));
	printf("size of uint is %lu\n", sizeof (unsigned int ));
	printf("Test num is %lu\n", 0xFFlu);

	unsigned int testint = 8;
	char testChar = 8 & 0xFFlu;
	testint = (unsigned int) testChar;
	printf("Now testint is %u\n", testint);
	return 0;
}
