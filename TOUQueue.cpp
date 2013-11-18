
#include "TOUQueue.h"
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


#define DEFAULT_SLEEP_PERIOD 5
TOUQueue::TOUQueue() {
	largest = -1;
	q_mutex = PTHREAD_MUTEX_INITIALIZER;
}

bool TOUQueue::add(int byteStart, unsigned len, long delta) {
	long now = getCurrentSeconds();
	QueueEntry newEntry(byteStart, len, now + delta);
	pthread_mutex_lock(&q_mutex);
	q.push_back(newEntry);
	this->len += len;
	pthread_mutex_unlock(&q_mutex);
	return true;
}

bool TOUQueue::removeBeforeByte(unsigned byteStart) {
	pthread_mutex_lock(&q_mutex);
	for(deque<QueueEntry>::iterator it = q.begin(); it != q.end();) {
		QueueEntry cur = *it;
		this->len -= cur.len;
		if (cur.byteStart < byteStart) {
			it = q.erase(it);
		} else {
			++it;
		}
	}
	pthread_mutex_unlock(&q_mutex);
	return true;
}

void TOUQueue::removeFront() {
	pthread_mutex_lock(&q_mutex);
	q.pop_front();
	pthread_mutex_unlock(&q_mutex);
}

/**
 * if the queue is not empty
 * front of the queue is written into the space pointer by *entry
 * and true is returned, otherwise false is returned
 */
bool TOUQueue::getFront(QueueEntry * entry) {
	bool result = false;
	pthread_mutex_lock(&q_mutex);
	if(!q.empty()) {
		*entry = q.front();
		result = true;
	}
	pthread_mutex_unlock(&q_mutex);
	return result;
}


bool TOUQueue::isEmpty() {
	bool result;
	pthread_mutex_lock(&q_mutex);
	result = q.empty();
	pthread_mutex_unlock(&q_mutex);
	return result;
}

long TOUQueue::getCurrentSeconds() {
	timeval now;
	if (gettimeofday(&now, NULL) == -1) {
		return -1;
	} else {
		perror("warning: gettimeofday() failed");
		return (long) now.tv_sec;
	}
}



list<QueueEntry> TOUQueue::getExpired(long dueTime) {
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

int TOUQueue::getLen() {
	int result;
	pthread_mutex_lock(&q_mutex);
	result = len;
	pthread_mutex_unlock(&q_mutex);
	return result;
}
