#ifndef TOUSEGMENT_H
#define TOUSEGMENT_H


#define TOU_HEADER_SIZE 12
#define TOU_ACK_OFFSET 4
#define TOU_CHSUM_OFFSET 8
#define TOU_RCVW_OFFSET 10
#define TOU_FIN_MASK 0b00000001
#define TOU_SYN_MASK 0b00000010
#define TOU_ACK_MASK 0b00010000


class TOUSegment {
private:
	unsigned int sequenceNum, ACKNum;
	unsigned short rcvWindow;
	bool is_fin, is_ack, is_syn;
	char * data;
	static unsigned int readInt(char * str);
	static unsigned short readShort(char * str);
public:
	TOUSegment();
	TOUSegment(unsigned int sequenceNum, unsigned int ACKNum, unsigned short rcvWindow,
					bool is_fin, bool is_ack, bool is_syn);
	static TOUSegment parseSegment(char *data, int len);
	unsigned int getSequenceNum();
	unsigned int getACKNum();
	bool isAck();
	bool isFin();
	bool isSyn();
	char * getData();
	static void putInt(char * array, unsigned int num);
	static void putShort(char * array, unsigned short num);
	bool putHeader(char * array);
};

#endif
