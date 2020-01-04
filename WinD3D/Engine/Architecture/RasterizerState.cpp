#include "RasterizerState.h"
#include "GraphicsThrows.m"
#include <Engine/Architecture/Codex.h>


RasterizerState::RasterizerState(Graphics& gfx, bool twoSided)
	:
	twoSided(twoSided)
{
	INFOMAN(gfx);

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	rasterDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.AntialiasedLineEnable = FALSE;

	GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer));
}

void RasterizerState::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->RSSetState(pRasterizer.Get());
}

std::shared_ptr<RasterizerState> RasterizerState::Resolve(Graphics& gfx, bool twoSided)
{
	return Codex::Resolve<RasterizerState>(gfx, twoSided);
}
std::string RasterizerState::GenerateUID(bool twoSided)
{
	using namespace std::string_literals;
	return typeid(RasterizerState).name() + "#"s + (twoSided ? "2s" : "1s");
}
std::string RasterizerState::GetUID() const noexcept
{
	return GenerateUID(twoSided);
}