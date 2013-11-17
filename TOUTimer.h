#include <pthread.h>
#include <deque>

using namespace std;

struct QueueEntry {
	unsigned byteNum;
	long dueTime;

};


class TOUTimer {
private:
	pthread_mutex_t mutex;
	deque<QueueEntry> q;
	unsigned largest;
	unsigned smallest;

public:
	TOUTimer();
	bool add(int byteNum, long delta);
	bool removeBeforeByte(unsigned byteNum);
	void removeFront();
	QueueEntry getFront();
	bool isEmpty();
	static long getCurrentSeconds();
};

