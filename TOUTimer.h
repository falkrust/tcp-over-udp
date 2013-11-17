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


class TOUTimer {
private:
	pthread_mutex_t q_mutex;
	deque<QueueEntry> q;
	unsigned largest;
	unsigned smallest;

public:
	TOUTimer();
	bool add(int byteNum, long delta);
	bool removeBeforeByte(unsigned byteNum);
	void removeFront();
	bool getFront(QueueEntry * entry);
	bool isEmpty();
	static long getCurrentSeconds();
	list<QueueEntry> getExpired(long dueTime);
};

#endif
