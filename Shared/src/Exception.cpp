#include <Shared/Exception.h>
#include <Shared/Log.h>
#include <Shared/WinSetup.h>
#include <format>


using namespace ver;

exception::exception(std::source_location sl, bool write) noexcept
	:sl(std::move(sl))
{
	if(write)WriteToOutput();
}

std::string exception::origin() const noexcept
{
	return std::format("[File]: {}\n[Line]: {}\n[Function]: {}", file(), line(), function());
}
const char* exception::what() const noexcept
{
	if (whatBuffer.empty())
		whatBuffer = std::format("{}\n{}", type(), origin());
	return whatBuffer.c_str();
}

void ver::exception::WriteToOutput()const noexcept
{
	ver::std_critical(what());
	//OutputDebugStringA(what());
}



//Window Exception
hr_exception::hr_exception(winrt::hresult hr, std::source_location sl)
	:exception(sl, false), hResult(hr)
{
	WriteToOutput();
}
std::string hr_exception::description() const noexcept
{
	wil::unique_hlocal_ansistring msgBuf;
	DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(msgBuf.put()),
		0, nullptr);

	if (nMsgLen == 0)
		return "Unknown error";
	std::string errorString = msgBuf.get();
	return errorString;
}
const char* hr_exception::what() const noexcept
{
	if (whatBuffer.empty())
	{
		whatBuffer = std::format(
			"{}\n[Error Code]: 0x{:08X}({})\n"
			"[Description]: {}\n{}",
			type(), (unsigned long)error_code(), (unsigned long)error_code(),
			description(), origin()
		);
	}
	return whatBuffer.c_str();
}