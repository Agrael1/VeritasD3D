#pragma once
#include <Engine/Bindable/Bindable.h>

namespace ver
{
	class Topology : public Bindable
	{
	public:
		Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
			:
			type(type)
		{}
	public:
		void Bind(Graphics& gfx) noxnd override
		{
			Bind(*GetContext(gfx));
		}
		void Bind(ID3D11DeviceContext& context) noxnd override
		{
			context.IASetPrimitiveTopology(type);
		}
	protected:
		D3D11_PRIMITIVE_TOPOLOGY type;
	};

	class LightTopology : public ver::GraphicsResource
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
