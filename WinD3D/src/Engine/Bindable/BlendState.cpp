#include <Engine/Bindable/BlendState.h>
#include <Engine/Bindable/Codex.h>
#include <format>
#include <d3d11.h>
#include <Shared/Checks.h>

using namespace ver;

BlendState::BlendState(Graphics& gfx, bool blending, std::optional<float> factors_in)
	:
	blending(blending)
{
	if (factors_in)
	{
		factors.emplace();
		factors->fill(*factors_in);
	}

	D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
	auto& brt = blendDesc.RenderTarget[0];
	if (blending)
	{
		brt.BlendEnable = TRUE;

		if (factors_in)
		{
			brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
			brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
		}
		else
		{
			brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		}
	}
	ver::check_hresult(GetDevice(gfx)->CreateBlendState(&blendDesc, pBlendState.put()));
}

void ver::BlendState::Bind(ID3D11DeviceContext& context) noxnd
{
	const float* data = factors ? factors->data() : nullptr;
	context.OMSetBlendState(pBlendState.get(), data, 0xFFFFFFFFu);
	ver::check_context();
}

void BlendState::SetFactor(float factor) noxnd
{
	assert(factors);
	return factors->fill(factor);
}

float BlendState::GetFactor() const noxnd
{
	assert(factors);
	return factors->front();
}

std::shared_ptr<BlendState> BlendState::Resolve(Graphics& gfx, bool blending, std::optional<float> factor)
{
	return Codex::Resolve<BlendState>(gfx, blending, factor);
}
std::string BlendState::GenerateUID(bool blending, std::optional<float> factor)
{
	return std::format("{}#{}{}", typeid(BlendState).name(), blending ? 'b' : 'n', factor ? "#f" + std::to_string(*factor) : "");
}
std::string BlendState::GetUID() const noexcept
{
	return GenerateUID(blending, factors ? factors->front() : std::optional<float>{});
}