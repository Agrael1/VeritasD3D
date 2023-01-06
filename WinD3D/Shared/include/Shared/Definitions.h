#pragma once
#include <winrt/Windows.Foundation.h>

namespace ver
{
	inline constexpr const bool debug_mode = DEBUG_MODE;
	using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;
} 

