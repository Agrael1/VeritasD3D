#pragma once
#include <Engine/Bindable/Bindable.h>
#include <array>

namespace ver
{
	class ShadowRasterizer : public Bindable
	{
	public:
		ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp);
	public:
		void ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		void Bind(Graphics& gfx) noxnd override;
		int GetDepthBias() const
		{
			return depthBias;
		}
		float GetSlopeBias() const
		{
			return slopeBias;
		}
		float GetClamp() const
		{
			return clamp;
		}
	protected:
		winrt::com_ptr<ID3D11RasterizerState> pRasterizer;
		int depthBias;
		float slopeBias;
		float clamp;
	};
}