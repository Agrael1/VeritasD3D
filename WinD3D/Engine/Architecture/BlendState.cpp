#include "BlendState.h"
#include <Engine/Architecture/Codex.h>
#include "GraphicsThrows.m"


BlendState::BlendState(Graphics& gfx, bool blending)
	:blendingMode(blending)
{
	INFOMAN(gfx);

	D3D11_BLEND_DESC blendDesc = {};
	auto& brt = blendDesc.RenderTarget[0];
	if (blendingMode)
	{
		brt.BlendEnable = TRUE;
		brt.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		brt.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_DEST_ALPHA;
		brt.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		brt.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		brt.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		brt.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		brt.BlendEnable = FALSE;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlendState));
}

void BlendState::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->OMSetBlendState(pBlendState.Get(), nullptr, 0xFFFFFFFFu);
}
std::string BlendState::GetUID() const noexcept
{
	return GenerateUID(blendingMode);
}

std::shared_ptr<BlendState> BlendState::Resolve(Graphics& gfx, bool blending)
{
	return Codex::Resolve<BlendState>(gfx, blending);
}
std::string BlendState::GenerateUID(bool Blending)
{
	return std::string(typeid(BlendState).name()) + "#" + (Blending ? "+" : "-");
}
