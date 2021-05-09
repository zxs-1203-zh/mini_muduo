#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <inttypes.h>
#include <string>
#include <utility>
#include "copyable.h"

namespace mini_muduo
{

using std::string;

class Timestamp: copyable
{

public:
	Timestamp():
		microSecondsFromEpoch_(0)
	{ }

	Timestamp(int64_t microSecondsFromEpoch):
		microSecondsFromEpoch_(microSecondsFromEpoch)
	{ }

	bool valid()const
	{
		return microSecondsFromEpoch_ > 0;
	}

	void swap(Timestamp &other)
	{
		std::swap(
				microSecondsFromEpoch_,
				other.microSecondsFromEpoch_
				);
	}

	int64_t microSecondsFromEpoch()const
	{
		return microSecondsFromEpoch_;
	}

	time_t secondsFromEpoch()const
	{
		return microSecondsFromEpoch_ *
			Timestamp::kMicroSecondsPerSecond;
	}

	string toString()const;

	string toFormattedString(bool showMicroSeconds)const;

	static Timestamp invalid()
	{
		return Timestamp();
	}

	static Timestamp now();

	static Timestamp fromUnixTime(time_t seconds)
	{
		return fromUnixTime(seconds, 0);
	}

	static Timestamp fromUnixTime(time_t seconds,
			int64_t microSeconds)
	{
		return Timestamp(
				seconds * Timestamp::kMicroSecondsPerSecond +
				microSeconds);
	}

	const static int kMicroSecondsPerSecond = 
		1000 * 1000;

private:
	int64_t microSecondsFromEpoch_;

};//class Timestamp

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsFromEpoch() <
		rhs.microSecondsFromEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsFromEpoch() ==
		lhs.microSecondsFromEpoch();
}

inline double timeDifference(
		Timestamp high, 
		Timestamp low
		)
{
	return static_cast<double>(
			high.secondsFromEpoch() - 
			low.secondsFromEpoch());
}

inline Timestamp addTime(Timestamp tm, double seconds)
{
	int64_t delta = static_cast<int64_t>(
			seconds * Timestamp::kMicroSecondsPerSecond);

	return Timestamp(tm.microSecondsFromEpoch() + delta);
}

}//namespace mini_muduo

#endif
