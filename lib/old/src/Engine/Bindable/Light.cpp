#include <Engine/Bindable/Light.h>
#include <Engine/Graphics.h>

ver::LightBuffer::LightBuffer(Graphics& gfx)
	:cbuf(gfx, binding_slot)
{
}

void ver::LightBuffer::Bind(Graphics& gfx) noxnd
{
	auto view = gfx.GetCamera();

	for (size_t i = 0; i < data.count; i++) //copy only valid
	{
		data.lights[i] = light_buffer[i];
		const auto pos = DirectX::XMLoadFloat4A(&data.lights[i].pos);
		DirectX::XMStoreFloat4A(&data.lights[i].pos, DirectX::XMVector3Transform(pos, view));
	}
	cbuf.Update(gfx, data);
	cbuf.Bind(gfx);
}

ver::PointLightConsts& ver::LightBuffer::Allocate()
{
	return light_buffer[data.count++];
}
