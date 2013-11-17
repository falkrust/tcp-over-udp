#include "TOUSegment.h"
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <arpa/inet.h>

using namespace std;

class IllegalData: public exception {
	virtual const char* what() const throw() {
		return "Received illegal data";
	}
} illegalData;

TOUSegment::TOUSegment() {
	this->data = NULL;
}

TOUSegment::TOUSegment(unsigned int sequenceNum, unsigned int ACKNum, unsigned short rcvWindow,
					bool is_fin, bool is_ack, bool is_syn) {
	this->sequenceNum = sequenceNum;
	this->ACKNum = ACKNum;
	this->rcvWindow = rcvWindow;
	this->is_fin = is_fin;
	this->is_ack = is_ack;
	this->is_syn = is_syn;
}

TOUSegment TOUSegment::parseSegment(char *data, int len) {
	if (len < TOU_HEADER_SIZE) {
		printf("Throwing an exception\n");
		throw illegalData;

	}
	printf("len is %d\n", len);
	TOUSegment newSegment;
	newSegment.data = &data[TOU_HEADER_SIZE];
	newSegment.sequenceNum = readInt(data);
	newSegment.ACKNum = readInt(&data[TOU_ACK_OFFSET]);
	newSegment.rcvWindow = readShort(&data[TOU_RCVW_OFFSET]);
	short chAndFlags = readShort(&data[TOU_CHSUM_OFFSET]);
	newSegment.is_ack = (chAndFlags & TOU_ACK_MASK) != 0;
	newSegment.is_syn =  (chAndFlags & TOU_SYN_MASK) != 0;
	newSegment.is_fin = (chAndFlags & TOU_FIN_MASK) != 0;

	printf("Sequence num is %d\n", newSegment.sequenceNum);

	printf("Got data %s\n", data);

	return newSegment;
}

unsigned int TOUSegment::getSequenceNum() {
	return sequenceNum;	
}

unsigned int TOUSegment::getACKNum() {
	return ACKNum;
}

bool TOUSegment::isAck() {
	return is_ack;
}

bool TOUSegment::isFin() {
	return is_fin;
}

bool TOUSegment::isSyn() {
	return is_syn;
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
		charPtr[0] = str[0];
		charPtr[1] = str[1];
		charPtr[2] = str[2];
		charPtr[3] = str[3];
	#elif __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
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
		array[3] = charPtr[0];
		array[2] = charPtr[1];
		array[1] = charPtr[2];
		array[0] = charPtr[3];
	#elif __BYTE_ORDER == __ORDER_BIG_ENDIAN__
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

bool TOUSegment::putHeader(char * array) {
	if (array == NULL) {
		return false;
	} else {
		putInt(array, sequenceNum);
		putInt(&array[TOU_ACK_OFFSET], ACKNum);
		short checksumAndFlag = 0;
		if (is_ack) {
			checksumAndFlag |= TOU_ACK_MASK;
		}
		if (is_fin) {
			checksumAndFlag |= TOU_FIN_MASK;
		}
		if (is_syn) {
			checksumAndFlag |= TOU_SYN_MASK;
		}
		putShort(&array[TOU_CHSUM_OFFSET], checksumAndFlag);
		putShort(&array[TOU_RCVW_OFFSET], rcvWindow);
		return true;
	}
}
