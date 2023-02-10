#include <Engine/Bindable/ShadowRasterizer.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

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

		INFOMAN(gfx);

		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.DepthBias = depthBias;
		rasterDesc.SlopeScaledDepthBias = slopeBias;
		rasterDesc.DepthBiasClamp = clamp;

		GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, pRasterizer.put()));
	}

	void ShadowRasterizer::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(pRasterizer.get()));
	}
}