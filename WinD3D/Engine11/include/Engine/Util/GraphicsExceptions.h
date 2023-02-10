#pragma once
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/Exception.h>

namespace ver
{
	class hr_error :public exception
	{
	public:
		hr_error(winrt::hresult hr, std::vector<std::string> messages = {}, std::source_location sl = std::source_location::current())noexcept;
		hr_error(HRESULT hr, std::vector<std::string> messages = {}, std::source_location sl = std::source_location::current())noexcept
			:hr_error(winrt::hresult(hr), messages, sl)
		{}
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

	class device_error : public hr_error
	{
		using hr_error::hr_error;
	public:
		std::string_view type()const noexcept override
		{
			return "Graphics Device Exception [Device Removed](DXGI_ERROR_DEVICE_REMOVED)";
		}
	};


	class context_error : public exception
	{
	public:
		context_error(std::vector<std::string> messages = {}, std::source_location sl = std::source_location::current())noexcept;
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


	inline void check_hresult(winrt::hresult hr, std::source_location sl = std::source_location::current())
	{
		if (hr < 0)throw make_error<hr_error>({ hr, {}, sl });
	}

	template<int = 1>requires(bool(DEBUG_MODE))
		inline void check_graphics(winrt::hresult hr, std::source_location sl = std::source_location::current())
	{
		if (hr < 0)throw make_error<hr_error>({ hr, DXGIInfoManager::GetMessages(), sl });
	}

	template<int = 1>requires(!bool(DEBUG_MODE))
		inline void check_graphics(winrt::hresult hr, std::source_location sl = std::source_location::current()) { ver::check_hresult(hr, sl); }

	template<int = 1>requires(bool(DEBUG_MODE))
		inline void check_device_remove(winrt::hresult hr, ID3D11Device* pDevice, std::source_location sl = std::source_location::current())
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw ver::make_error<ver::device_error>({ pDevice->GetDeviceRemovedReason(), DXGIInfoManager::GetMessages() });
		ver::check_graphics(hr);
	}

	template<int = 1>requires(!bool(DEBUG_MODE))
		inline void check_device_remove(winrt::hresult hr, ID3D11Device* pDevice, std::source_location sl = std::source_location::current()) { ver::check_hresult(hr, sl); }

	template<int = 1>requires(bool(DEBUG_MODE))
		inline void check_context(std::source_location sl = std::source_location::current()) {
		if (DXGIInfoManager::GetNumMessages())
			throw make_error<context_error>({ DXGIInfoManager::GetMessages(), sl });
	}

	template<int = 1>requires(!bool(DEBUG_MODE))
		inline void check_context() {}
}
