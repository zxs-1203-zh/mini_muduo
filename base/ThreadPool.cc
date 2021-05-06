#include <assert.h>
#include <memory>
#include <mutex>

#include "ThreadPool.h"


namespace mini_muduo
{

ThreadPool::ThreadPool(const string& nameArg):
	maxSize_(0),
	name_(nameArg),
	running(false)
	{ }

ThreadPool::~ThreadPool()
{
	if(running)
	{
		stop();
	}
}

void ThreadPool::setMaxQueueSize(int maxSize)
{
	maxSize_ = maxSize;
}

void ThreadPool::setThreadInitCallBack(const Task& cb)
{
	threadInitCallBack_ = cb;
}

void ThreadPool::start(int numThreads)
{
	assert(!running);
	running = true;

	threads_.reserve(numThreads);

	char buf[32];
	for(int i = 0; i < numThreads; ++i)
	{
		
		snprintf(buf, sizeof(buf), "%d", numThreads);

		threads_.emplace_back(new Thread(
					std::bind(&ThreadPool::runInthread, this),
					name_ + buf));
		threads_[i]->start();

	}
}

void ThreadPool::stop()
{
	running = false;
	for(auto& thread: threads_)
	{
		thread->join();
	}
}

void ThreadPool::run(const Task& task)
{
	if(threads_.empty())
	{
		task();
		return;
	}

	std::unique_lock<std::mutex> lk(mut_);

	notFull_.wait(lk, [this]{return !this->isFull();});

	tasks_.push_back(task);

	notEmpty_.notify_one();
}

void ThreadPool::runInthread()
{
	try
	{
		if(threadInitCallBack_)
		{
			threadInitCallBack_();
		}

		while(running)
		{
			Task task(take());
			if(task)
			{
				task();
			}
		}
	}
	catch(...)
	{
		throw;
	}
}

bool ThreadPool::isFull()
{
	//when this function is called, the thread have already
	//locked the mut_
	
	return tasks_.size() >= maxSize_;
}

ThreadPool::Task ThreadPool::take()
{
	Task task;

	std::unique_lock<std::mutex> lk;
	
	notEmpty_.wait(lk, 
			[this]{
				return !tasks_.empty() || !running;
			});

	if(!tasks_.empty())
	{
		task = tasks_.front();
		tasks_.pop_front();
		notFull_.notify_one();
	}
	return task;
}

}//mini_muduo
