#include "CountDownLatch.h"
#include <mutex>

using namespace muduo;


CountDownLatch::CountDownLatch(int count):
	count_(count) {}

void CountDownLatch::wait()
{
	std::unique_lock<std::mutex> lk(mut_);

	cond_.wait(lk, [this]{return this->count_ == 0;});
}

void CountDownLatch::countDown()
{
	std::lock_guard<std::mutex> lk(mut_);
	
	--count_;
	if(count_ == 0)
	{
		cond_.notify_all();
	}
}

int CountDownLatch::getCount()
{
	std::lock_guard<std::mutex> lk(mut_);

	return count_;
}
