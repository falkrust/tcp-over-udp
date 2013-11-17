#ifndef TOUTIMER_H
#define TOUTIMER_H

#include <pthread.h>
#include <deque>
#include <list>
using namespace std;

struct QueueEntry {
	unsigned byteStart, len;
	long dueTime;
	QueueEntry(unsigned bs, unsigned ln, long dt) {
		byteStart = bs;
		len = ln;
		dueTime = dt;
	};
	QueueEntry(){};
};


class TOUQueue {
private:
	pthread_mutex_t q_mutex;
	deque<QueueEntry> q;
	unsigned largest;
	unsigned smallest;
	int len;

public:
	TOUQueue();
	bool add(int byteStart, unsigned len, long delta);
	bool removeBeforeByte(unsigned byteNum);
	void removeFront();
	bool getFront(QueueEntry * entry);
	bool isEmpty();
	static long getCurrentSeconds();
	list<QueueEntry> getExpired(long dueTime);
	int getLen();
};

#endif
