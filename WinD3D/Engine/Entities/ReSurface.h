#pragma once
#include <Framework\GdiSetup.h>
#include <Framework\Exception.h>

class ReSurface
{
	class LoadException : public Exception
	{
	public:
		LoadException(int line, const char* file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
public:
	ReSurface(const std::wstring& filepath);
	UINT GetWidth()noexcept;
	UINT GetHeight()noexcept;
	bool UsesAlpha()const noexcept;
	Gdiplus::BitmapData& GetBufferPtr()noexcept;
private:
	Gdiplus::Bitmap bitmap;
	Gdiplus::BitmapData buffer;
	bool HasAlphaD = false;
};

#undef FULL_WINOPT