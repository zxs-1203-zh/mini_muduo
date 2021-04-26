#ifndef NOCOPYABLE_H
#define NOCOPYABLE_H

namespace mini_muduo
{
	class nocopyable
	{

	public:
		nocopyable(const nocopyable&) = delete;
		nocopyable& operator=(const nocopyable&) = delete;

	protected:
		nocopyable() = default;
		~nocopyable() = default;

	};//class nocopyable

} //namespace mini_muduo

#endif
