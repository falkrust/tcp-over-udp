#include "TOUSegment.h"
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <arpa/inet.h>

using namespace std;

#define HEADER_SIZE 12
#define BYTE_MASK 0xFFlu

class IllegalData: public exception {
	virtual const char* what() const throw() {
		return "Received illegal data";
	}
} illegalData;

TOUSegment::TOUSegment() {

}

TOUSegment TOUSegment::parseSegment(char *data, int len) {
	try {
		throw illegalData;
	} catch (exception& e){
		printf("Illegal exception caught\n");
	}
	if (len < HEADER_SIZE || data == NULL) {
		throw illegalData;
	}

	TOUSegment newSegment;
	newSegment.data = data;
	char * current = data;
	newSegment.sequenceNum = readInt(current);
	printf("Sequence num is %d\n", newSegment.sequenceNum);

	printf("Got data %s\n", data);

	return TOUSegment();
}

unsigned int TOUSegment::getSequenceNum() {
	return 0;	
}

unsigned int TOUSegment::getACKNum() {
	return 0;
}

bool TOUSegment::isACK() {
	return false;
}

bool TOUSegment::isFin() {
	return false;
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

bool TOUSegment::bigEndian = true;

bool TOUSegment::findEndiannes() {
    bigEndian = (htonl(47) == 47); 
    return bigEndian;

}