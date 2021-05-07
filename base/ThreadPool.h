#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <bits/c++config.h>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <string>
#include <deque>
#include <vector>

#include "Thread.h"

namespace mini_muduo
{

	using std::string;

class ThreadPool
{

public:
	typedef std::function<void()> Task;

	explicit ThreadPool(const string& nameArg = string("Threadpool"));
	~ThreadPool();

	void setMaxQueueSize(int maxSize);

	void setThreadInitCallBack(const Task& cb);

	void start(int numThreads);

	void stop();

	void run(Task task);

	const string& name()const
	{
		return name_;
	}

	std::size_t queueSize()const
	{
		std::lock_guard<std::mutex> lk(mut_);
		return tasks_.size();
	}

private:
	void runInthread();
	bool isFull()const;
	Task take();

	Task threadInitCallBack_;

	mutable std::mutex mut_;
	std::condition_variable notFull_;
	std::condition_variable notEmpty_;
	std::size_t maxSize_;
	string name_;
	bool running;

	std::deque<Task> tasks_;
	std::vector<std::unique_ptr<Thread> > threads_;

};//ThreadPool

}//mini_muduo

#endif
