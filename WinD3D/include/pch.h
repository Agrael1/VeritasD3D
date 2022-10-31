#pragma once
#include <Winrt/Windows.Foundation.h>
#include <Engine/Deprecated/noexcept_if.h>
#include <wil/wrl.h>
#include <d3d11.h>

namespace winrt
{
	using Windows::Foundation::IAsyncAction;
}

template<class C>
inline auto** array_view(winrt::com_ptr<C>& self)
{
	return reinterpret_cast<C**>(&self);
}

#undef min
#undef max