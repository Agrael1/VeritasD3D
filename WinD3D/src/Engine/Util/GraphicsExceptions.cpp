#include <Engine/Util/GraphicsExceptions.h>
#include <format>
#include <DxErr.h>

using namespace ver;

hr_error::hr_error(winrt::hresult hr, std::vector<std::string> messages, std::source_location sl) noexcept
	:exception(sl), hr(hr)
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

std::string_view hr_error::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}
std::string_view hr_error::GetErrorDescription() const noexcept
{
	return DXGetErrorDescription(hr);
}
const char* hr_error::what()const noexcept
{
	if (whatBuffer.empty())
	{
		whatBuffer = std::format("{}\n"
			"[Error Code]: {:#08X} ({})\n"
			"[Error String]: {}\n"
			"[Description]: {}\n", type(), (unsigned long)GetErrorCode(), (unsigned long)GetErrorCode(), GetErrorString(), GetErrorDescription());
		if (!info.empty())	whatBuffer += std::format("\n[Error Info]:\n{}\n\n", GetErrorInfo());

		whatBuffer += origin();
	}
	return whatBuffer.c_str();
}





ContextException::ContextException(std::vector<std::string> messages, std::source_location sl) noexcept
	:exception(sl)
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
		whatBuffer = type();
		if (!info.empty())
		{
			whatBuffer += std::format("\n[Error Info]:\n{}\n\n", GetErrorInfo());
		}
		whatBuffer += origin();
	}

	return whatBuffer.c_str();
}
