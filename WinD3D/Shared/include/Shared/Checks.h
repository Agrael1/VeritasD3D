#pragma once
#ifndef MODULE
#include <winrt/base.h>
#include <Shared/Defines.h>
#include <source_location>
#endif

SHARED_EXPORT namespace ver
{
	void check_static(bool throw_error = true, std::source_location sl = std::source_location::current());
	inline void check_hresult(winrt::hresult hr, std::source_location sl = std::source_location::current())
	{
		if (hr < 0)check_static(true, sl);
	}
	inline bool check_hresult_nothrow(winrt::hresult hr, std::source_location sl = std::source_location::current())
	{
		if (hr < 0)
		{
			check_static(false, sl);
			return false;
		}
		return true;
	}
}