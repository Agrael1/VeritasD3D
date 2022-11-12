#pragma once
#include <Framework\GdiSetup.h>
#include <Framework/Exception.h> 

class ReSurface
{
	class LoadException : public Exception
	{
	public:
		LoadException(int line, const char* file, std::string_view filepath, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
public:
	ReSurface(std::string_view filepath);
public:
	UINT GetWidth()const noexcept;
	UINT GetHeight()const noexcept;
	UINT GetStride()const noexcept;
	bool UsesAlpha()const noexcept;
	void* GetBufferPtr()const noexcept;
private:
	Gdiplus::Bitmap bitmap;
	Gdiplus::BitmapData buffer;
	bool HasAlphaD = false;
};

#undef FULL_WINOPT