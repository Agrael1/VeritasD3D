#pragma once
#include <Graphics.h>

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext(const Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DXGIInfoManager& GetInfoManager(Graphics& gfx);
};