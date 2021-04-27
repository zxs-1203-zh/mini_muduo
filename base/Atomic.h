#ifndef ATOMIC_H
#define ATOMIC_H

#include "noncopyable.h"
#include <stdint.h>

namespace mini_muduo
{
	
namespace detail
{
template<typename T>
class AtomicInteger : noncopyable
{

public:
	AtomicInteger():
		value(0) {}

	T get()
	{
		return __atomic_load_n(&value, __ATOMIC_SEQ_CST);
	}

	T getAndAdd(T x)
	{
		return __atomic_fetch_add(&value, __ATOMIC_SEQ_CST);
	}

	T addAndGet(T x)
	{
		return getAndAdd(x) + x;
	}

	T incrementAndGet()
	{
		return addAndGet(1);
	}

	T decrementAndGet()
	{
		return addAndGet(-1);
	}

	void add(T x)
	{
		getAndAdd(x);
	}

	void increment()
	{
		incrementAndGet();
	}

	void decrement()
	{
		decrementAndGet();
	}

	T getAndSet(T x)
	{
		return __atomic_exchange_n(&value, x, __ATOMIC_SEQ_CST);
	}

private:
	volatile T value;

};
}
	typedef detail::AtomicInteger<int32_t> AtomicInt32;
	typedef detail::AtomicInteger<int64_t> AtomicInt64;
}

#endif
