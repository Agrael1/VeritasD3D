#include "Texture.h"
#include "GraphicsThrows.m"
#include "Codex.h"
#include <Framework/Utility.h>
#include <Entities/Surface.h>

constexpr uint32_t DefaultDifTexture = 0xffffffff;
constexpr uint32_t DefaultNrmTexture = 0xff8080ff;
constexpr uint32_t DefaultSpcTexture = 0xffffffff;


Texture::Texture(Graphics& gfx, std::string_view path, UINT slot)
	:slot(slot), path(path)
{
	INFOMAN(gfx);

	Surface s;
	if (!s.FromFile(path))
	{
		ResolveToDefault(gfx);
		return;
	}


	hasAlpha = s.UsesAlpha();

	//create texture resource
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = s.GetWidth();
	texDesc.Height = s.GetHeight();
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	GFX_THROW_INFO(DirectX::CreateShaderResourceView(GetDevice(gfx),
		s->GetImages(), s->GetImageCount(),
		s->GetMetadata(), &pTextureView
	));
}

void Texture::Bind(Graphics& gfx)noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf()));
}
std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, std::string_view path, UINT slot)
{
	return Codex::Resolve<Texture>(gfx, path, slot);
}
concurrency::task<std::shared_ptr<Texture>>
Texture::ResolveAsync(Graphics& gfx, std::string path, UINT slot)
{
	return concurrency::create_task(
		[&gfx, path, slot]() {
			return Codex::Resolve<Texture>(gfx, path, slot); 
		});
}
std::string Texture::GenerateUID(std::string_view path, UINT slot)
{
	using namespace std::string_literals;
	return typeid(Texture).name() + "#"s + path.data() + "#" + std::to_string(slot);
}
std::string Texture::GetUID() const noexcept
{
	return GenerateUID(path, slot);
}

bool Texture::UsesAlpha() const noexcept
{
	return hasAlpha;
}
UINT Texture::CalculateNumberOfMipLevels(UINT width, UINT height) noexcept
{
	const float xSteps = std::ceil(log2((float)width));
	const float ySteps = std::ceil(log2((float)height));
	return (UINT)std::max(xSteps, ySteps);
}

void Texture::ResolveToDefault(Graphics& gfx)
{
	INFOMAN(gfx);
	hasAlpha = false;

	//create texture resource
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = 1;
	texDesc.Height = 1;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&texDesc,
		nullptr,
		&pTexture));

	const uint32_t* pmap = nullptr;

	switch (slot)
	{
	case 0:pmap = &DefaultDifTexture; break;
	case 1:pmap = &DefaultSpcTexture; break;
	case 2:pmap = &DefaultNrmTexture; break;
	default:
		pmap = &DefaultDifTexture; break;
	}

	GetContext(gfx)->UpdateSubresource(
		pTexture.Get(), 0u, nullptr, pmap, sizeof(*pmap), 0u
	);

	// create a resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView
	(
		pTexture.Get(), &srvDesc, &pTextureView
	));
}
