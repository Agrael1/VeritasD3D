#include <Engine/Bindable/Sampler.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <format>

ver::Sampler::Sampler(Graphics& gfx, Type type, bool reflect, uint32_t slot)
	:
	type(type),
	reflect(reflect),
	slot(slot)
{
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

	ver::check_graphics(GetDevice(gfx)->CreateSamplerState(&samplerDesc, pSampler.put()));
}

void ver::Sampler::Bind(Graphics& gfx) noxnd{
	Bind(*GetContext(gfx));
}
void ver::Sampler::Bind(ID3D11DeviceContext& context) noxnd {
	context.PSSetSamplers(slot, 1, array_view(pSampler));
	ver::check_context();
}

std::shared_ptr<ver::Sampler> ver::Sampler::Resolve(Graphics& gfx, Type type, bool reflect, uint32_t slot)
{
	return ver::Codex::Resolve<Sampler>(gfx, type, reflect, slot);
}
std::string ver::Sampler::GenerateUID(Type type, bool reflect, uint32_t slot)
{
	return std::format("Sampler#{}{}{}", (int)type, reflect ? 'R' : 'W', slot);
}
std::string ver::Sampler::GetUID() const noexcept
{
	return GenerateUID(type, reflect, slot);
}

void ver::DomainSampler::Bind(Graphics& gfx) noxnd {
	Bind(*GetContext(gfx));
}void ver::VertexSampler::Bind(Graphics& gfx) noxnd {
	Bind(*GetContext(gfx));
}void ver::HullSampler::Bind(Graphics& gfx) noxnd {
	Bind(*GetContext(gfx));
}

void ver::DomainSampler::Bind(ID3D11DeviceContext& context) noxnd
{
	context.DSSetSamplers(slot, 1, array_view(pSampler));
	ver::check_context();
}
void ver::VertexSampler::Bind(ID3D11DeviceContext& context) noxnd
{
	context.VSSetSamplers(slot, 1, array_view(pSampler));
	ver::check_context();
}
void ver::HullSampler::Bind(ID3D11DeviceContext& context) noxnd
{
	context.HSSetSamplers(slot, 1, array_view(pSampler));
	ver::check_context();
}