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
		
		snprintf(buf, sizeof(buf), "%d", i + 1);

		threads_.emplace_back(new Thread(
					std::bind(&ThreadPool::runInthread, this),
					name_ + buf));
		threads_[i]->start();
	}

	if(numThreads == 0 && threadInitCallBack_)
	{
		threadInitCallBack_();
	}
}

void ThreadPool::stop()
{
	{
		std::lock_guard<std::mutex> lk(mut_);
		running = false;
		notEmpty_.notify_all();
		notFull_.notify_all();
	}
	for(auto& thread: threads_)
	{
		thread->join();
	}
}

void ThreadPool::run(Task task)
{
	if(threads_.empty())
	{
		task();
		return;
	}

	std::unique_lock<std::mutex> lk(mut_);

	notFull_.wait(lk, [this]{return !(this->isFull() && running);});

	if(!running)
	{
		return;
	}

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
			Task task = take();
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

bool ThreadPool::isFull()const
{
	//when this function is called, the thread have already
	//locked the mut_
	
	return maxSize_ > 0 && tasks_.size() >= maxSize_;
}

ThreadPool::Task ThreadPool::take()
{
	Task task;

	std::unique_lock<std::mutex> lk(mut_);
	
	notEmpty_.wait(lk, 
			[this]{
				return !(this->tasks_.empty() && this->running);
			});

	if(!tasks_.empty())
	{
		task = tasks_.front();
		tasks_.pop_front();
		if(maxSize_ > 0)
		{
			notFull_.notify_one();
		}
	}
	return task;
}

}//mini_muduo
