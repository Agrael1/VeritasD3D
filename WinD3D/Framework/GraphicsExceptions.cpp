#include "GraphicsExceptions.h"
#include <fmt\printf.h>
#include <Framework\dxerr.h>

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
	char buf[512];
	DXGetErrorDescription(hr, buf, 512u);
	return buf;
}
std::string HrException::GetErrorInfo() const noexcept
{
	return info;
}
const char* HrException::what()const noexcept
{
	std::string report = fmt::sprintf("%s\n"
		"[Error Code]: 0x%X (%lu)\n"
		"[Error String]: %s\n"
		"[Description]: %s\n", GetType(), GetErrorCode(), (unsigned long)GetErrorCode(), GetErrorString(), GetErrorDescription());

	if (!info.empty())	report += fmt::sprintf("\n [Error Info]:\n%s\n\n", GetErrorInfo());

	report += GetOriginString();
	whatBuffer = std::move(report);
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
	std::string report = GetType();
	if (!info.empty())
	{
		report += fmt::sprintf("\n [Error Info]:\n%s\n\n", GetErrorInfo());
	}
	report += GetOriginString();
	whatBuffer = std::move(report);
	return whatBuffer.c_str();
}
const char* ContextException::GetType()const noexcept
{
	return "Veritas Graphics Info Exception";
}