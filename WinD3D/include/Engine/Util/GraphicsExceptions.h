#pragma once
#include <Engine/Util/Exception.h>
#include <vector>

namespace ver
{
	class hr_error :public exception
	{
	public:
		hr_error(winrt::hresult hr, std::vector<std::string> messages = {}, std::source_location sl = std::source_location::current())noexcept;
	public:
		const char* what()const noexcept override;
		std::string_view type()const noexcept override
		{
			return "Veritas Graphics Exception";
		}
		winrt::hresult GetErrorCode()const noexcept
		{
			return hr;
		}
		std::string_view GetErrorInfo()const noexcept
		{
			return info;
		}
		std::string_view GetErrorString()const noexcept;
		std::string_view GetErrorDescription()const noexcept;
	private:
		winrt::hresult hr;
		std::string info;
	};

	class DeviceRemovedException : public hr_error
	{
		using hr_error::hr_error;
	public:
		std::string_view type()const noexcept override
		{
			return "Graphics Device Exception [Device Removed](DXGI_ERROR_DEVICE_REMOVED)";
		}
	};


	class ContextException : public exception
	{
	public:
		ContextException(std::vector<std::string> messages = {}, std::source_location sl = std::source_location::current())noexcept;
	public:
		const char* what()const noexcept override;
		std::string_view type()const noexcept override
		{
			return "Veritas Graphics Info Exception";
		}
		std::string GetErrorInfo()const noexcept;
	private:
		std::string info;
	};


	inline void check_hresult(winrt::hresult hr)
	{
		if (hr < 0)throw make_error<hr_error>(hr);
	}
}
