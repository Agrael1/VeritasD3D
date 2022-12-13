#include <Engine/Bindable/Sampler.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

Sampler::Sampler(Graphics& gfx, Type type, bool reflect, uint32_t slot)
	:
	type(type),
	reflect(reflect),
	slot(slot)
{
	INFOMAN(gfx);

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.Filter = [type]() {
		switch (type)
		{
		case Type::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
		case Type::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
		default:
		case Type::Bilinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}();
	samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

	GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
}

void Sampler::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetSamplers(slot, 1, pSampler.GetAddressOf()));
}
std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, Type type, bool reflect, uint32_t slot)
{
	return ver::Codex::Resolve<Sampler>(gfx, type, reflect, slot);
}
std::string Sampler::GenerateUID(Type type, bool reflect, uint32_t slot)
{
	using namespace std::string_literals;
	return typeid(Sampler).name() + "#"s + std::to_string((int)type) + (reflect ? "R"s : "W"s) + std::to_string(slot);
}
std::string Sampler::GetUID() const noexcept
{
	return GenerateUID(type, reflect, slot);
}

void DomainSampler::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->DSSetSamplers(slot, 1, pSampler.GetAddressOf()));
}