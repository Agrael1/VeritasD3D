#include "ReSurface.h"
#include <sstream>

#pragma comment( lib,"gdiplus.lib" )

ReSurface::ReSurface(const std::wstring& filepath)
	:bitmap(filepath.c_str())
{
	if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Loading image [" << std::string(filepath.begin(), filepath.end()) << "]: failed to load.";
		throw LoadException(__LINE__, __FILE__, ss.str());
	}	
	
	bitmap.LockBits(&Gdiplus::Rect(0, 0, GetWidth(), GetHeight()), Gdiplus::ImageLockModeRead,
	PixelFormat32bppARGB, &buffer);
}

UINT ReSurface::GetWidth()noexcept
{
	return bitmap.GetWidth();
}
UINT ReSurface::GetHeight() noexcept
{
	return bitmap.GetHeight();
}

Gdiplus::BitmapData& ReSurface::GetBufferPtr() noexcept
{
	return buffer;
}

// surface exception stuff
ReSurface::LoadException::LoadException(int line, const char* file, std::string note) noexcept
	:
Exception(line, file),
note(std::move(note))
{}
const char* ReSurface::LoadException::what() const noexcept
{
	std::ostringstream oss;
	oss << Exception::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
const char* ReSurface::LoadException::GetType() const noexcept
{
	return "Graphics Loading Exception";
}
const std::string& ReSurface::LoadException::GetNote() const noexcept
{
	return note;
}