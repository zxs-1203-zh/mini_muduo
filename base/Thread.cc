#include <cstdio>
#include <ctime>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <assert.h>
#include <utility>
#include "Thread.h"
#include "CurrentThread.h"
#include "Timestamp.h"

namespace mini_muduo
{

namespace detail
{

int gettid()
{
	return static_cast<int>(::syscall(SYS_gettid));
}

void afterFork()
{
	CurrentThread::t_cachedTid = 0;
	CurrentThread::t_threadName = "main";
	CurrentThread::tid();
}

class ThradNameInitializer
{

public:
	ThradNameInitializer()
	{
		mini_muduo::CurrentThread::t_threadName = "main";
		CurrentThread::tid();
		pthread_atfork(NULL, NULL, &afterFork);
	}
};
ThradNameInitializer init;

class ThreadData
{

public:
	typedef Thread::ThreadFunc ThreadFunc;

	ThreadData(ThreadFunc fuc, pid_t* tid,
			CountDownLatch *latch, string name):
		fuc_(fuc),
		tid_(tid),
		latch_(latch),
		name_(name)
	{ }

	void runInThread()
	{
		*tid_ = CurrentThread::tid();
		tid_ = NULL;
		latch_->countDown();
		latch_ = NULL;

		CurrentThread::t_threadName = name_.empty() 
			? "mini_muduoThread" : name_.c_str();
		::prctl(PR_SET_NAME, CurrentThread::t_threadName);

		try
		{
			fuc_();
		}
		catch(...)
		{
			throw;
		}
	}

private:
	ThreadFunc fuc_;
	pid_t* tid_;
	CountDownLatch* latch_;
	string name_;

};

void* startThread(void *obj)
{
	ThreadData* data = static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
	return NULL;
}

}//detail

void CurrentThread::cacheTid()
{
	if(t_cachedTid ==0)
	{
		t_cachedTid = detail::gettid();
		t_tidStringLength = ::snprintf(
				t_tidString,
				sizeof(t_tidString),
				"%5d",
				t_cachedTid);
	}
}

Thread::Thread(ThreadFunc fuc, const string& n):
	started_(false),
	joined_(false),
	pthreadId_(0),
	tid_(0),
	name_(n),
	latch_(1),
	fuc_(fuc)
{
	setDefaultName();
}

Thread::~Thread()
{
	if(!joined_)
	{
		pthread_detach(pthreadId_);
	}
}

void Thread::setDefaultName()
{
	int num = numCreated_.incrementAndGet();
	if(name_.empty())
	{
		char buf[32];
		snprintf(buf, sizeof buf, "Thread%d", num);
		name_ = buf;
	}
}

bool CurrentThread::isMainThread()
{
	return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
	struct timespec tm = {0, 0};
	tm.tv_sec = static_cast<time_t>(
			usec / Timestamp::kMicroSecondsPerSecond);
	tm.tv_nsec = static_cast<time_t>(
			usec % Timestamp::kMicroSecondsPerSecond * 1000);
	::nanosleep(&tm, NULL);
}

AtomicInt32 Thread::numCreated_;

void Thread::start()
{
	assert(!started_);
	started_ = true;
	detail::ThreadData *data = new detail::ThreadData(
			std::move(fuc_), &tid_, &latch_, name_);
	if(pthread_create(&pthreadId_, NULL, &detail::startThread,
				static_cast<void *>(data)))
	{
		delete data;
		started_ = false;
		//make it use log
		fprintf(stderr, "pthread_create error\n");
	}
	else
	{
		latch_.wait();
		assert(tid_ > 0);
	}
}

int Thread::join()
{
	assert(started_);
	assert(!joined_);
	joined_ = true;
	return pthread_join(pthreadId_, NULL);
}

}//mini_muduo
