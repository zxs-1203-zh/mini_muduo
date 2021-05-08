#include <unistd.h>

#include "ProcessInfo.h"


namespace mini_muduo
{

namespace ProcessInfo
{

pid_t pid()
{
	return ::getpid();
}

string pidString()
{
	char buf[32];
	snprintf(buf, sizeof buf, "%d", pid());
	return buf;
}

string hostname()
{
	char buf[256];
	if(::gethostname(buf, sizeof buf) == 0)
	{
		buf[sizeof(buf) - 1] = '\0';
		return buf;
	}

	return string("unknownhost");
}

}//ProcessInfo

}//mini_muduo
