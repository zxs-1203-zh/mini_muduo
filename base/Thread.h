#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <string>
#include <functional>
#include "CountDownLatch.h"
#include "Atomic.h"
#include "noncopyable.h"


namespace mini_muduo
{

using std::string;

class Thread: noncopyable
{

public:
	typedef std::function<void()> ThreadFunc;

	explicit Thread(ThreadFunc, const string &name = string());
	~Thread();

	bool stared()
	{
		return joined_;
	}

	void start();
	int join();

	pid_t tid()const
	{
		return tid_;
	}

	const string& name()const
	{
		return name_;
	}

	static int numCreated()
	{
		return numCreated_.get();
	}

private:
	void setDefaultName();
	
	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	string name_;
	mini_muduo::CountDownLatch latch_;
	ThreadFunc fuc_;

	static mini_muduo::AtomicInt32 numCreated_;

};

}

#endif
