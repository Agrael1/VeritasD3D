#pragma once
#include <DirectXTex.h>

class Surface
{
public:
	Surface(std::string_view filepath);
public:
	UINT GetWidth()const noexcept;
	UINT GetHeight()const noexcept;
	UINT GetStride()const noexcept;
	bool UsesAlpha()const noexcept;
	void* GetBufferPtr()const noexcept;
private:
	DirectX::ScratchImage image;
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
};