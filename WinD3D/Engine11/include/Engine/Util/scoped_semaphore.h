#pragma once
#include <semaphore>

namespace ver
{
	class scoped_semaphore
	{
	public:
		scoped_semaphore(std::binary_semaphore& a) :a(a) { a.acquire(); }
		~scoped_semaphore() { a.release(); }
	private:
		std::binary_semaphore& a;
	};
}