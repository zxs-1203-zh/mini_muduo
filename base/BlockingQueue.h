// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_BLOCKINGQUEUE_H
#define MUDUO_BASE_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include <assert.h>

#include "noncopyable.h"

namespace mini_muduo
{

template<typename T>
class BlockingQueue : noncopyable
{
 public:
  BlockingQueue()
    : mutex_(),
      notEmpty_(),
      queue_()
  {
  }

  void put(const T& x)
  {
	std::lock_guard<std::mutex> lk(mutex_);
    queue_.push_back(x);
    notEmpty_.notify_one(); // wait morphing saves us
    // http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
  }

  void put(T&& x)
  {
    std::lock_guard<std::mutex> lk(mutex_);
    queue_.push_back(std::move(x));
    notEmpty_.notify_one();
  }

  T take()
  {
	std::unique_lock<std::mutex> lk(mutex_);
    // always use a while-loop, due to spurious wakeup
   	notEmpty_.wait(lk, [&]{return !queue_.empty();});

    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return front;
  }

  size_t size() const
  {
	std::lock_guard<std::mutex> lk(mutex_);
    return queue_.size();
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable  notEmpty_;
  std::deque<T>     queue_;
};

}  // namespace mini_muduo

#endif  // MUDUO_BASE_BLOCKINGQUEUE_H
