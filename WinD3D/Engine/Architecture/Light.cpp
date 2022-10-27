#include "Light.h"
#include "Graphics.h"

ver::LightBuffer::LightBuffer(Graphics& gfx)
	:cbuf(gfx, binding_slot)
{
}

winrt::IAsyncAction ver::LightBuffer::InitializeAsync(Graphics& gfx)
{
	co_await cbuf.InitializeAsync(gfx, binding_slot);
}

void ver::LightBuffer::Bind(Graphics& gfx)
{
	auto view = gfx.GetCamera();
	DirectX::XMStoreFloat4x4A(&data.view_matrix, view);
	cbuf.Update(gfx, data);
	cbuf.Bind(gfx);
}

ver::PointLightConsts& ver::LightBuffer::Allocate()
{
	return data.lights[data.count++];
}
