#pragma once
#include <pplawait.h>

namespace ver
{
	template <typename T>
	concept IsAsync =
		requires(T t) {
			{ t.InitializeAsync() } -> std::same_as<winrt::IAsyncAction>;
	};

	template<class C, class ... Args>
	concurrency::task<std::shared_ptr<C>> make_shared_async(Args&&... args)
	{
		co_await winrt::resume_background();
		auto x = std::make_shared<C>();
		co_await x->InitializeAsync(std::forward<Args>(args)...);
		co_return x;
	}
	template<class C, class ... Args>
	concurrency::task<std::unique_ptr<C>> make_unique_async(Args&&... args)
	{
		auto x = std::make_unique<C>();
		co_await x->InitializeAsync(std::forward<Args>(args)...);
		co_return x;
	}
}