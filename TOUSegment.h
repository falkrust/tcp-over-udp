#ifndef TOUSEGMENT_H
#define TOUSEGMENT_H

class TOUSegment {
private:
	unsigned int sequenceNum, ACKNum;
	unsigned short rcvWindow;
	bool is_fin, is_ack;
	char * data;
	static unsigned int readInt(char * str);
	static unsigned short readShort(char * str);
public:
	TOUSegment();
	static TOUSegment parseSegment(char *data, int len);
	unsigned int getSequenceNum();
	unsigned int getACKNum();
	bool isACK();
	bool isFin();
	char * getData();
	static void putInt(char * array, unsigned int num);
	static void putShort(char * array, unsigned short num);
	bool putHeader(char * array);
};

#endif