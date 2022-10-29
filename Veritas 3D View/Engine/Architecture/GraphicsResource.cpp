#include "pch.h"
#include "GraphicsResource.h"
#include <stdexcept>


ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& gfx) noexcept
{
	return gfx.pContext.get();
}

ID3D11Device* GraphicsResource::GetDevice(Graphics& gfx) noexcept
{
	return gfx.pDevice.get();
}

DXGIInfoManager& GraphicsResource::GetInfoManager(Graphics& gfx)
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
}