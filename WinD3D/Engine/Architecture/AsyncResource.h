#pragma once
#include <memory>
#include <pplawait.h>
#include <concepts>
#include "pch.h"

namespace ver
{
	template<class R>
	class AsyncResource
	{
	public:
		template<class ...Args>
		winrt::IAsyncAction InitializeAsync(Args&&... args)
		{
			co_await winrt::resume_background();
			static_cast<R*>(this)->Initialize(std::forward<Args>(args)...);
		}
	};

	template<class C, class ... Args>
	concurrency::task<std::shared_ptr<C>> make_shared_async(Args&&... args)
	{
		auto x = std::make_shared<C>();
		co_await x->InitializeAsync(std::forward<Args>(args)...);
		co_return x;
	}
}