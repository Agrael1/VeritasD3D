#pragma once
#include <winrt/Windows.Foundation.h>
#include <span>

#define noxnd noexcept(!DEBUG_MODE)

namespace ver
{
	inline constexpr const bool debug_mode = DEBUG_MODE;
	inline constexpr const bool dx12_enabled = DX12_BACKEND;
	using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;

	template<class C>
	inline auto** array_view(winrt::com_ptr<C>& self)
	{
		return reinterpret_cast<C**>(&self);
	}

	inline IAsyncAction when_all(std::span<IAsyncAction> tasks)
	{
		for (auto& i : tasks)
			co_await i;
	}
}