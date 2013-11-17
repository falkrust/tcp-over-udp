
#include "TOUTimer.h"
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


#define DEFAULT_SLEEP_PERIOD 5
TOUTimer::TOUTimer() {
	largest = -1;
	q_mutex = PTHREAD_MUTEX_INITIALIZER;
}

bool TOUTimer::add(int byteStart, long delta) {
	long now = getCurrentSeconds();
	QueueEntry newEntry;
	newEntry.byteStart = byteStart;
	newEntry.dueTime = delta + now;
	pthread_mutex_lock(&q_mutex);
	q.push_back(newEntry);
	pthread_mutex_unlock(&q_mutex);
	return true;
}

bool TOUTimer::removeBeforeByte(unsigned byteStart) {
	pthread_mutex_lock(&q_mutex);
	for(deque<QueueEntry>::iterator it = q.begin(); it != q.end();) {
		QueueEntry cur = *it;
		if (cur.byteStart <= byteStart) {
			it = q.erase(it);
		} else {
			++it;
		}
	}
	pthread_mutex_unlock(&q_mutex);
	return true;
}

void TOUTimer::removeFront() {
	pthread_mutex_lock(&q_mutex);
	q.pop_front();
	pthread_mutex_unlock(&q_mutex);
}

/**
 * if the queue is not empty
 * front of the queue is written into the space pointer by *entry
 * and true is returned, otherwise false is returned
 */
bool TOUTimer::getFront(QueueEntry * entry) {
	bool result = false;
	pthread_mutex_lock(&q_mutex);
	if(!q.empty()) {
		*entry = q.front();
		result = true;
	}
	pthread_mutex_unlock(&q_mutex);
	return result;
}


bool TOUTimer::isEmpty() {
	bool result;
	pthread_mutex_lock(&q_mutex);
	result = q.empty();
	pthread_mutex_unlock(&q_mutex);
	return result;
}

long TOUTimer::getCurrentSeconds() {
	timeval now;
	if (gettimeofday(&now, NULL) == -1) {
		return -1;
	} else {
		perror("warning: gettimeofday() failed");
		return (long) now.tv_sec;
	}
}



list<QueueEntry> TOUTimer::getExpired(long dueTime) {
	list<QueueEntry> result;
	pthread_mutex_lock(&q_mutex);
	while(!q.empty()) {
		printf("Inside handler\n");
		QueueEntry front = q.front();
		if (front.dueTime <= dueTime) {
			result.push_back(front);
			q.pop_front();	
		} else {
			printf("Queue wasn't empty\n");
			// TODO: possibly need to check everything
			// if items not ordered by due time
			break;
		}
	}
	pthread_mutex_unlock(&q_mutex);
	return result;
}
