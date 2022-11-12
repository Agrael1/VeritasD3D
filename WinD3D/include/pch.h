#pragma once
#include <Winrt/Windows.Foundation.h>
#include <Engine/Deprecated/noexcept_if.h>
#include <wil/wrl.h>
#include <d3d11_4.h>
#include <span>

namespace winrt
{
	using Windows::Foundation::IAsyncAction;
}

template<class C>
inline auto** array_view(winrt::com_ptr<C>& self)
{
	return reinterpret_cast<C**>(&self);
}

namespace ver
{
	inline winrt::IAsyncAction when_all(std::span<::winrt::IAsyncAction> tasks)
	{
		for (auto& i : tasks)
			co_await i;
	}
}

#ifdef DEBUG_MODE
#include <Engine/Util/Timer.h>
#endif // DEBUG_MODE


#undef min
#undef max