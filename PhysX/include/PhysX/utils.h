#pragma once
#include <memory>

namespace ver::ph
{
	template<class T>
	struct physx_deleter
	{
		void operator()(T* r) {
			r->release();
		}
	};

	template <class T>
	using physx_ptr = std::unique_ptr<T, physx_deleter<T>>;
}