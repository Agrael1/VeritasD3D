#include <Engine/Bindable/BlendState.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <format>

using namespace ver;

BlendState::BlendState(Graphics& gfx, bool blending, std::optional<float> factors_in)
	:
	blending(blending)
{
	Initialize(gfx, blending, factors_in);
}

winrt::IAsyncAction ver::BlendState::InitializeAsync(Graphics& gfx, bool blending, std::optional<float> factor)
{
	co_await winrt::resume_background();
	Initialize(gfx, blending, factor);
}

void ver::BlendState::Initialize(Graphics& gfx, bool blending, std::optional<float> factor)
{
	this->blending = blending;
	if (factor)
	{
		factors.emplace();
		factors->fill(*factor);
	}

	D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
	auto& brt = blendDesc.RenderTarget[0];
	if (blending)
	{
		brt.BlendEnable = TRUE;

		if (factor)
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
	ver::check_graphics(GetDevice(gfx)->CreateBlendState(&blendDesc, pBlendState.put()));
}

void BlendState::Bind(Graphics& gfx) noxnd
{
	Bind(*GetContext(gfx));
}

void ver::BlendState::Bind(ID3D11DeviceContext& context) noxnd
{
	const float* data = factors ? factors->data() : nullptr;
	context.OMSetBlendState(pBlendState.get(), data, 0xFFFFFFFFu);
	check_context();
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
concurrency::task<std::shared_ptr<BlendState>> ver::BlendState::ResolveAsync(Graphics& gfx, bool blending, std::optional<float> factor)
{
	return Codex::ResolveAsync<BlendState>(gfx, blending, factor);
}
std::string BlendState::GenerateUID(bool blending, std::optional<float> factor)
{
	return std::format("{}#{}{}", typeid(BlendState).name(), blending ? 'b' : 'n', factor ? "#f" + std::to_string(*factor) : "");
}
std::string BlendState::GetUID() const noexcept
{
	return GenerateUID(blending, factors ? factors->front() : std::optional<float>{});
}