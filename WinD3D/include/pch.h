#pragma once
#include <Winrt/Windows.Foundation.h>
#include <Engine/Deprecated/noexcept_if.h>
#include <wil/wrl.h>
#include <d3d11.h>

namespace winrt
{
	using Windows::Foundation::IAsyncAction;
}

#undef min
#undef max