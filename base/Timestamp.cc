#include <bits/stdint-intn.h>
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include "Timestamp.h"

namespace mini_muduo
{

string Timestamp::toString()const
{
	char buf[32] = {0};

	int64_t seconds = microSecondsFromEpoch_ / 
		Timestamp::kMicroSecondsPerSecond;
	int64_t microSeconds = microSecondsFromEpoch_ %
		Timestamp::kMicroSecondsPerSecond;

	::snprintf(buf, sizeof(buf), "%ld.%ld",
			seconds, microSeconds);

	return buf;
}

string Timestamp::toFormattedString(bool showMicroSeconds)const
{
	struct tm tm_time;
	char buf[64] = {0};

	time_t seconds = static_cast<time_t>(
			microSecondsFromEpoch_ / 
			kMicroSecondsPerSecond
			);

	::gmtime_r(&seconds, &tm_time);
	
	if(showMicroSeconds)
	{
		int microSeconds = microSecondsFromEpoch_ %
			kMicroSecondsPerSecond;
		::snprintf(
				buf, 
				sizeof(buf),
				"%4d%02d%02d %02d:%02d:%02d.%06d",
				tm_time.tm_year + 1900,
				tm_time.tm_mon + 1,
				tm_time.tm_mday,
				tm_time.tm_hour,
				tm_time.tm_min,
				tm_time.tm_sec,
				microSeconds
				);
	}
	else
	{
		snprintf(
				buf, 
				sizeof(buf), 
				"%4d%02d%02d %02d:%02d:%02d",
				tm_time.tm_year + 1900, 
				tm_time.tm_mon + 1, 
				tm_time.tm_mday,
				tm_time.tm_hour,
				tm_time.tm_min,
				tm_time.tm_sec
				);
	}

	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;
	::gettimeofday(&tv, NULL);

	int64_t seconds = static_cast<int64_t>(tv.tv_sec);
	int64_t microSeconds = static_cast<int64_t>(tv.tv_usec);

	return Timestamp(
			seconds * kMicroSecondsPerSecond +
			microSeconds
			);
}

}//namespace mini_muduo
