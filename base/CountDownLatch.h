#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>


namespace muduo
{

class CountDownLatch
{

public:
	explicit CountDownLatch(int count);

	void wait();

	void countDown();

	int getCount();

private:
	mutable std::mutex mut_;
	std::condition_variable cond_;
	int count_;

};

}

#endif
