#include "TOUSegment.h"
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <arpa/inet.h>

using namespace std;

#define HEADER_SIZE 12
#define ACK_OFFSET 4
#define RCV_WINDOW_OFFSET 10
#define BYTE_MASK 0xFFlu
#define ACK_MASK 0b10000000
#define FIN_MASK 0b01000000


class IllegalData: public exception {
	virtual const char* what() const throw() {
		return "Received illegal data";
	}
} illegalData;

TOUSegment::TOUSegment() {
	this->data = NULL;
}

TOUSegment TOUSegment::parseSegment(char *data, int len) {
	if (len < HEADER_SIZE) {
		throw illegalData;
	}
	TOUSegment newSegment;
	newSegment.data = &data[HEADER_SIZE];
	newSegment.sequenceNum = readInt(data);
	newSegment.ACKNum = readInt(&data[ACK_OFFSET]);
	newSegment.rcvWindow = readShort(&data[RCV_WINDOW_OFFSET]);
	printf("Sequence num is %d\n", newSegment.sequenceNum);

	printf("Got data %s\n", data);

	return TOUSegment();
}

unsigned int TOUSegment::getSequenceNum() {
	return sequenceNum;	
}

unsigned int TOUSegment::getACKNum() {
	return ACKNum;
}

bool TOUSegment::isACK() {
	return is_ack;
}

bool TOUSegment::isFin() {
	return is_fin;
}

char * TOUSegment::getData() {
	return NULL;
}

unsigned int TOUSegment::readInt(char * str) {
	if (str == NULL) {
		perror("readLong(): NULL argument\n");
		return 0;
	}

	unsigned int result;
	char * charPtr = (char*) &result;

	// read everything in char array
	// according to endianness
	// the code relies on __BYTE_ORDER__ macro
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		printf("Inside BIG endian\n");
		charPtr[0] = str[0];
		charPtr[1] = str[1];
		charPtr[2] = str[2];
		charPtr[3] = str[3];
	#elif __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
		printf("Inside LITTLE endian\n");
		charPtr[3] = str[0];
		charPtr[2] = str[1];
		charPtr[1] = str[2];
		charPtr[0] = str[3];
	#else
		perror("readInt(): Could not determine endiannes of the system");
		result = 0;
	#endif

	return result;
}

void TOUSegment::putInt(char * array, unsigned int num) {
	if (array == NULL) {
		perror("putInt(): NULL argument");
		return;
	}
	// write everything in char array
	// according to endianness
	char * charPtr = (char*) &num;
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		printf("Inside LITTLE endian\n");
		array[3] = charPtr[0];
		array[2] = charPtr[1];
		array[1] = charPtr[2];
		array[0] = charPtr[3];
	#elif __BYTE_ORDER == __ORDER_BIG_ENDIAN__
		printf("Inside BIG endian\n");
		array[0] = charPtr[0];
		array[1] = charPtr[1];
		array[2] = charPtr[2];
		array[3] = charPtr[3];
	#else
		perror("putInt(): Could not determine endiannes of the system");
		return;
	#endif
}




unsigned short TOUSegment::readShort(char * str) {
	if (str == NULL) {
		perror("readLong(): NULL argument\n");
		return 0;
	}
	unsigned short result;
	char * charPtr = (char*) &result;


	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		charPtr[0] = str[0];
		charPtr[1] = str[1];
	#elif __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
		charPtr[1] = str[0];
		charPtr[0] = str[1];
	#else
		perror("readInt(): Could not determine endiannes of the system");
		result = 0;
	#endif
	return result;
}

void TOUSegment::putShort(char * array, unsigned short num) {
	if (array == NULL) {
		perror("putInt(): NULL argument");
		return;
	}
	char * charPtr = (char*) &num;
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		array[1] = charPtr[0];
		array[0] = charPtr[1];
	#elif __BYTE_ORDER == __ORDER_BIG_ENDIAN__
		array[0] = charPtr[0];
		array[1] = charPtr[1];
	#else
		perror("putInt(): Could not determine endiannes of the system");
		return;
	#endif
}
