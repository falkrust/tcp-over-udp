#ifndef TOUTIMER_H
#define TOUTIMER_H

#include <pthread.h>
#include <deque>

using namespace std;

struct QueueEntry {
	unsigned byteNum;
	long dueTime;

};


class TOUTimer {
private:
	pthread_mutex_t q_mutex;
	pthread_mutex_t *s_mutex;
	int sockfd;
	deque<QueueEntry> q;
	unsigned largest;
	unsigned smallest;
	char * data;

public:
	TOUTimer();
	bool add(int byteNum, long delta);
	bool removeBeforeByte(unsigned byteNum);
	void removeFront();
	bool getFront(QueueEntry * entry);
	bool isEmpty();
	void setData(char * data);
	void setSocketMutex(pthread_mutex_t * s_mutex);
	void setSocket(int sockfd);
	static long getCurrentSeconds();
	static void* clientHandler(void * timerObj);
};

#endif
