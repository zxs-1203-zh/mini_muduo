#include <type_traits>
#include "CurrentThread.h"

namespace mini_muduo
{

namespace CurrentThread
{

__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread const char* t_threadName = "unknown";
__thread int t_tidStringLength = 6;

static_assert(std::is_same<int, pid_t>::value,
		"pid_t should be int");

string stackTrace(bool damage)
{
	return string();
}

}//CurrentThread

}//mini_muduo
