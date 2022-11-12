#pragma once
#include <Engine/Util/Exception.h>


namespace ver
{
	class window_exception :public ver::exception
	{
	public:
		window_exception(winrt::hresult hr, std::source_location sl = std::source_location::current());
	public:
		const char* what() const noexcept override;
		std::string_view type()const noexcept override
		{
			return "Vertas Window Exception";
		}
		winrt::hresult error_code() const noexcept
		{
			return hResult;
		}
		std::string description() const noexcept;
	private:
		winrt::hresult hResult;
	};
}


#define WND_EXCEPT( hr ) ver::make_error<ver::window_exception>({hr})
#define WND_LAST_EXCEPT()  ver::make_error<ver::window_exception>({(HRESULT)GetLastError()})
#define WND_CALL_INFO(call) if(!(call)) throw(WND_LAST_EXCEPT())