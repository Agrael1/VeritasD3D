#pragma once
#include <Shared/Exception.h>
#include <Shared/Definitions.h>
#include <winrt/base.h>


namespace ver
{
	// true if any errors
	bool log_dxgi_errors()noexcept;
	winrt::hresult last_windows_error() noexcept;


	inline void check_context(std::source_location sl = std::source_location::current())
	{
		if constexpr(debug_mode)
		if (log_dxgi_errors())
			throw ver::exception{ sl };
	}

	inline void check_bool(bool check, std::source_location sl = std::source_location::current())
	{
		if (check)return;
		log_dxgi_errors();
		throw ver::exception{ sl };
	}
	inline void check_hresult(winrt::hresult hr, std::source_location sl = std::source_location::current())
	{
		if (hr >= 0)return;
		log_dxgi_errors();
		throw ver::hr_exception{ hr, sl };
	}
	inline [[nodiscard]] bool check_hresult_nothrow(winrt::hresult hr, std::source_location sl = std::source_location::current())noexcept
	{
		bool a = hr >= 0;
		if(a) return true;
		log_dxgi_errors();
		return false;
	}
	inline void check_windows(bool check, std::source_location sl = std::source_location::current())
	{
		if (check)return;
		throw ver::hr_exception{ last_windows_error(), sl };
	}
}