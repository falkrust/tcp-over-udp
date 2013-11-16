#ifndef TOUSEGMENT_H
#define TOUSEGMENT_H

class TOUSegment {
private:
	unsigned int sequenceNum, ACKNum;
	bool fin, ACK;
	char * data;
	static unsigned int readInt(char * str);
	static bool bigEndian;
public:
	TOUSegment();
	static TOUSegment parseSegment(char *data, int len);
	unsigned int getSequenceNum();
	unsigned int getACKNum();
	bool isACK();
	bool isFin();
	char * getData();
	static void putInt(char * array, unsigned int num);
	static bool findEndiannes();
};

#endif