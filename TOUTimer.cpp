
#include "TOUTimer.h"
#include <sys/time.h>
#include <pthread.h>

TOUTimer::TOUTimer() {
	largest = -1;
	mutex = PTHREAD_MUTEX_INITIALIZER;
}

bool TOUTimer::add(int byteNum, long delta) {
	timeval now;
	if(gettimeofday(&now, NULL) == -1) {
		return false;
	} else {
		QueueEntry newEntry;
		newEntry.byteNum = byteNum;
		newEntry.dueTime = delta + (long)now.tv_sec;
		pthread_mutex_lock(&mutex);
		q.push_back(newEntry);
		pthread_mutex_unlock(&mutex);
		return true;
	}

}

bool TOUTimer::removeBeforeByte(unsigned byteNum) {
	pthread_mutex_lock(&mutex);
	for(deque<QueueEntry>::iterator it = q.begin(); it != q.end();) {
		QueueEntry cur = *it;
		if (cur.byteNum <= byteNum) {
			it = q.erase(it);
		} else {
			++it;
		}
	}
	pthread_mutex_unlock(&mutex);
	return true;
}

void TOUTimer::removeFront() {
	pthread_mutex_lock(&mutex);
	q.pop_front();
	pthread_mutex_unlock(&mutex);
}

QueueEntry TOUTimer::getFront() {
	QueueEntry result;
	pthread_mutex_lock(&mutex);
	result = q.front();
	pthread_mutex_unlock(&mutex);
	return QueueEntry();
}

bool TOUTimer::isEmpty() {
	bool result;
	pthread_mutex_lock(&mutex);
	result = q.empty();
	pthread_mutex_unlock(&mutex);
	return result;
}

long TOUTimer::getCurrentSeconds() {
	timeval now;
	if (gettimeofday(&now, NULL) == -1) {
		return -1;
	} else {
		return (long) now.tv_sec;
	}
}
