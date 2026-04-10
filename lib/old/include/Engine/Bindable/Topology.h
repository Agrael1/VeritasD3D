#pragma once
#include <Engine/Bindable/GraphicsResource.h>

namespace ver
{
	class Topology : public ver::GraphicsResource
	{
	public:
		static void Bind(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			Bind(*GetContext(gfx), type);
		}
		static void Bind(ID3D11DeviceContext& context, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			context.IASetPrimitiveTopology(type);
		}
	};
}
