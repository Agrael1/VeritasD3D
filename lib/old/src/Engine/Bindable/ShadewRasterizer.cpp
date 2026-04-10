#include <Engine/Bindable/ShadowRasterizer.h>
#include <Shared/Checks.h>

namespace ver
{
	ShadowRasterizer::ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp)
	{
		ChangeDepthBiasParameters(gfx, depthBias, slopeBias, clamp);
	}

	void ShadowRasterizer::ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp)
	{
		this->depthBias = depthBias;
		this->slopeBias = slopeBias;
		this->clamp = clamp;

		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.DepthBias = depthBias;
		rasterDesc.SlopeScaledDepthBias = slopeBias;
		rasterDesc.DepthBiasClamp = clamp;

		ver::check_hresult(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, pRasterizer.put()));
	}

	void ShadowRasterizer::Bind(Graphics& gfx) noxnd
	{
		(GetContext(gfx)->RSSetState(pRasterizer.get()));
		ver::check_context();
	}
}