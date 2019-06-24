#include "ReSurface.h"
#include <sstream>

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
