#include "TCPOverUDP.h"
#include <stdio.h>

int main(int argc, char ** argv) {
	char domainName[] = "localhost";
	int port = 8080;
	TOUClient client(domainName, port);
	printf("Client was created successfully\n");

	bool initRes = client.init();
	printf("Result is %d\n", initRes);

	bool connRes = client.connect();
	printf("Result is %d\n", connRes);

	return 0;
}
