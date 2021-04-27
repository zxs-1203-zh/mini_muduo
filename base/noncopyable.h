#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace mini_muduo
{

class noncopyable
{

public:
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;

protected:
	noncopyable() = default;
	~noncopyable() = default;

};//class noncopyable

} //namespace mini_muduo

#endif
