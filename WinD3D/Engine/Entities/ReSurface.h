#pragma once

#ifdef _DEBUG
#define GDISetup
#endif // DEBUG

#ifdef GDISetup
#include <Framework\GdiSetup.h>
#else
#include <dxtex/DirectXTex.h>
#endif
#include <Framework/Exception.h> 

class ReSurface
{
	class LoadException : public Exception
	{
	public:
		LoadException(int line, const char* file, std::string_view filepath, std::string note, HRESULT hr) noexcept;
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
#ifdef GDISetup
	Gdiplus::Bitmap bitmap;
	Gdiplus::BitmapData buffer;
#else
	DirectX::ScratchImage image;
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
#endif
	bool HasAlphaD = false;
};

#undef FULL_WINOPT