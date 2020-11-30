#include "GraphicsExceptions.h"
#include <fmt\printf.h>
#include "../packages/DXSDK_Missing.1.1.2/Include/DxErr.h"

namespace GFX
{
	HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> messages) noexcept
		:GException(line, file), hr(hr)
	{
		// join messages in a single string
		for (const auto& m : messages)
		{
			info += m;
			info.push_back('\n');
		}
		if (!info.empty())
		{
			info.pop_back();
		}
	}

	HRESULT HrException::GetErrorCode() const noexcept
	{
		return hr;
	}
	std::string HrException::GetErrorString() const noexcept
	{
		return DXGetErrorString(hr);
	}
	std::string HrException::GetErrorDescription() const noexcept
	{
		return DXGetErrorDescription(hr);
	}
	std::string HrException::GetErrorInfo() const noexcept
	{
		return info;
	}
	const char* HrException::what()const noexcept
	{
		if (whatBuffer.empty())
		{
			whatBuffer = fmt::sprintf("%s\n"
				"[Error Code]: 0x%X (%lu)\n"
				"[Error String]: %s\n"
				"[Description]: %s\n", GetType(), GetErrorCode(), (unsigned long)GetErrorCode(), GetErrorString(), GetErrorDescription());
			if (!info.empty())	whatBuffer += fmt::sprintf("\n [Error Info]:\n%s\n\n", GetErrorInfo());

			whatBuffer += GetOriginString();
		}
		return whatBuffer.c_str();
	}
	const char* HrException::GetType()const noexcept
	{
		return "Veritas Graphics Exception";
	}

	const char* DeviceRemovedException::GetType()const noexcept
	{
		return "Graphics Device Exception [Device Removed](DXGI_ERROR_DEVICE_REMOVED)";
	}

	ContextException::ContextException(int line, const char* file, std::vector<std::string> messages) noexcept
		:GException(line, file)
	{
		// join messages in a single string
		for (const auto& m : messages)
		{
			info += m;
			info.push_back('\n');
		}
		if (!info.empty())
		{
			info.pop_back();
		}
	}
	std::string ContextException::GetErrorInfo() const noexcept
	{
		return info;
	}
	const char* ContextException::what()const noexcept
	{
		if (whatBuffer.empty())
		{
			whatBuffer = GetType();
			if (!info.empty())
			{
				whatBuffer += fmt::sprintf("\n [Error Info]:\n%s\n\n", GetErrorInfo());
			}
			whatBuffer += GetOriginString();
		}

		return whatBuffer.c_str();
	}
	const char* ContextException::GetType()const noexcept
	{
		return "Veritas Graphics Info Exception";
	}
}