#include <Engine/Bindable/GraphicsResource.h>
#include <Engine/Graphics.h>

using namespace ver;

ID3D11DeviceContext* GraphicsResource::GetContext(const Graphics& gfx) noexcept
{
	return gfx.pContext.get();
}

ID3D11Device* GraphicsResource::GetDevice(const Graphics& gfx) noexcept
{
	return gfx.pDevice.get();
}

DXGIInfoManager* GraphicsResource::GetInfoManager(Graphics& gfx) noexcept
{
#ifndef NDEBUG
	return &gfx.infoManager;
#else
	return nullptr;
#endif
}