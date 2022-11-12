#include <fmt/printf.h>
#include "ReSurface.h"
#include <Engine/Window.h> //just for TranslateErrorCode
#include <Framework/Utility.h>

#include <sstream>




// surface exception stuff
ReSurface::LoadException::LoadException(int line, const char* file, std::string_view filepath, std::string Note) noexcept
	:
	Exception(line, file)
{
	using namespace std::string_literals;
	note = std::move("[Error]: "s + Note + "\n" + "[File]: " + filepath.data());
}
const char* ReSurface::LoadException::what() const noexcept
{
	whatBuffer = fmt::sprintf("%s:\n [Note]: %s ", Exception::what(), note);
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

ReSurface::ReSurface(std::string_view filepath)
	:bitmap(ToWide(filepath).c_str())
{
	if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Loading image [" << std::string(filepath.begin(), filepath.end()) << "]: failed to load.";
		throw LoadException(__LINE__, __FILE__, filepath, ss.str());
	}

	auto lock_rect = Gdiplus::Rect(0, 0, bitmap.GetWidth(), bitmap.GetHeight());

	bitmap.LockBits(&lock_rect, Gdiplus::ImageLockModeRead,
	PixelFormat32bppARGB, &buffer);

	byte* p = (byte*)buffer.Scan0;
	for (size_t i = 3u; (i < bitmap.GetHeight()*bitmap.GetWidth())&&(!HasAlphaD); i+=4)
	{		
		if (p[i]!=255)
		{
			HasAlphaD = true;
			break;
		}
	}
}

UINT ReSurface::GetWidth()const noexcept
{
	return buffer.Width;
}
UINT ReSurface::GetHeight()const noexcept
{
	return buffer.Height;
}
UINT ReSurface::GetStride() const noexcept
{
	return buffer.Stride;
}
bool ReSurface::UsesAlpha() const noexcept
{
	return HasAlphaD;
}

void* ReSurface::GetBufferPtr()const noexcept
{
	return buffer.Scan0;
}