#include <Engine/Util/WindowExceptions.h>
#include <format>

//Window Exception
HrException::HrException(int line, const char* file, HRESULT hr)
	:WindowException(line, file), hResult(hr)
{}
std::string WindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	wil::unique_hlocal_ansistring msgBuf;
	DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(msgBuf.put()),
		0, nullptr);

	if (nMsgLen == 0)
		return "Unknown error";
	std::string errorString = msgBuf.get();
	return errorString;
}
HRESULT HrException::GetErrorCode() const noexcept
{
	return hResult;
}
std::string HrException::GetErrorDescription() const noexcept
{
	return WindowException::TranslateErrorCode(hResult);
}
const char* HrException::GetType()const noexcept
{
	return "Vertas Window Exception";
}
const char* HrException::what() const noexcept
{
	if (whatBuffer.empty())
	{
		whatBuffer = std::format(
			"{}\n[Error Code]: 0x{}({})\n"
			"[Description]: {}\n{}",
			GetType(), GetErrorCode(), (unsigned long)GetErrorCode(),
			GetErrorDescription(), GetOriginString()
		);
	}
	return whatBuffer.c_str();
}
const char* NoGfxException::GetType() const noexcept
{
	return "Veritas Window Exception [No Graphics]";
}