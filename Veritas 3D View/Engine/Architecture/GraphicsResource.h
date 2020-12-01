#pragma once
#include <Graphics.h>

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DXGIInfoManager& GetInfoManager(Graphics& gfx);
};