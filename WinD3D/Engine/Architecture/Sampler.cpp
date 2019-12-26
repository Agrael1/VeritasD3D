#include "Sampler.h"
#include "GraphicsThrows.m"
#include <Engine/Architecture/Codex.h>

Sampler::Sampler(Graphics & gfx)
{
	INFOMAN(gfx);

	D3D11_SAMPLER_DESC sDesc = {};
	sDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sDesc, &pSampler));
}

void Sampler::Bind(Graphics& gfx)noexcept
{
	GetContext(gfx)->PSSetSamplers(0u, 1u, pSampler.GetAddressOf());
}
std::string Sampler::GetUID() const noexcept
{
	return GenerateUID();
}

std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx)
{
	return Codex::Resolve<Sampler>(gfx);
}
std::string Sampler::GenerateUID()
{
	return typeid(Sampler).name();
}
