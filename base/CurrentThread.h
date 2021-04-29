#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <string>

namespace mini_muduo
{

namespace CurrentThread
{

using std::string;

extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread const char* t_threadName;
extern __thread int t_tidStringLength;

void cacheTid();

inline int tid()
{
	if(t_cachedTid == 0)
	{
		cacheTid();
	}

	return t_cachedTid;
}

inline const char* tidString()
{
	return t_tidString;
}

inline const char* name()
{
	return t_threadName;
}

inline int tidStringLength()
{
	return t_tidStringLength;
}

bool isMainThread();

void sleepUsec(int64_t usec);

string stackTrace(bool damage);

} //CurrentThread

}//mini_mudo

#endif
