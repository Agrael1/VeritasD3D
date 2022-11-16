#pragma once
#include <Engine/Bindable/Bindable.h>
#include <d3d11_4.h>

namespace ver
{
	class Viewport : public Bindable
	{
	public:
		Viewport(Graphics& gfx)
			:
			Viewport(gfx, (float)gfx.GetWidth(), (float)gfx.GetHeight())
		{}
		Viewport(Graphics& gfx, float width, float height)
		{
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
		}
		void Bind(Graphics& gfx) noxnd override
		{
			GetContext(gfx)->RSSetViewports(1u, &vp);
		}
	private:
		D3D11_VIEWPORT vp{};
	};
}
