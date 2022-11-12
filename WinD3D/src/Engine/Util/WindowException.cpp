#include <Engine/Util/WindowExceptions.h>
#include <format>
#include <wil/win32_helpers.h>


using namespace ver;

//Window Exception
window_exception::window_exception(winrt::hresult hr, std::source_location sl)
	:exception(sl), hResult(hr)
{}
std::string window_exception::description() const noexcept
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
const char* window_exception::what() const noexcept
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