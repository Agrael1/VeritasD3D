#include "Sampler.h"
#include "GraphicsThrows.m"

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