#include <Engine/Bindable/Texture.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <DirectXTex.h>

using namespace ver;

constexpr uint32_t DefaultDifTexture = 0xffffffff;
constexpr uint32_t DefaultNrmTexture = 0xff8080ff;
constexpr uint32_t DefaultSpcTexture = 0xffffffff;
inline constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;

std::optional<DirectX::ScratchImage> LoadWICTexture(std::wstring_view path)
{
	DirectX::ScratchImage image;
	HRESULT hr = (DirectX::LoadFromWICFile(path.data(), DirectX::WIC_FLAGS_NONE, nullptr, image));
	if (hr < 0) return{};

	if (image.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*image.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (hr < 0) return{};
		image = std::move(converted);
	}

	DirectX::ScratchImage mipped;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipped);
	if (hr < 0) return{};
	return mipped;
}

Texture::Texture(Graphics& gfx, std::filesystem::path path, uint32_t slot)
	:slot(slot), path(std::move(path))
{
	Initialize(gfx);
}

winrt::IAsyncAction Texture::InitializeAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot)
{
	co_await winrt::resume_background();
	this->path = std::move(path);
	this->slot = slot;
	Initialize(gfx);
}
void Texture::Initialize(Graphics& gfx)
{
	auto texture = LoadWICTexture(path.c_str());
	if (!texture) return ResolveToDefault(gfx);

	hasAlpha = texture->IsAlphaAllOpaque();

	winrt::check_hresult(DirectX::CreateShaderResourceView(GetDevice(gfx),
		texture->GetImages(), texture->GetImageCount(),
		texture->GetMetadata(), pTextureView.put()
	));
}

void Texture::Bind(Graphics& gfx)noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, array_view(pTextureView)));
}
std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, std::filesystem::path path, uint32_t slot)
{
	return ver::Codex::Resolve<Texture>(gfx, std::move(path), slot);
}
concurrency::task<std::shared_ptr<Texture>>
Texture::ResolveAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot)
{
	return ver::Codex::ResolveAsync<Texture>(gfx, std::move(path), slot);
}


std::string Texture::GenerateUID(const std::filesystem::path& path, uint32_t slot)
{
	return std::format("{}#{}#{}",typeid(Texture).name(),path.string(),std::to_string(slot));
}
std::string Texture::GetUID() const noexcept
{
	return GenerateUID(path, slot);
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
	texDesc.MiscFlags = 0;

	winrt::com_ptr<ID3D11Texture2D> pTexture;


	const uint32_t* pmap = nullptr;

	switch (slot)
	{
	case 0:pmap = &DefaultDifTexture; break;
	case 1:pmap = &DefaultSpcTexture; break;
	case 2:pmap = &DefaultNrmTexture; break;
	default:
		pmap = &DefaultDifTexture; break;
	}

	D3D11_SUBRESOURCE_DATA sd = { 0 };
	sd.pSysMem = pmap;
	sd.SysMemPitch = 4;

	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&texDesc,
		&sd,
		pTexture.put()));

	// create a resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView
	(
		pTexture.get(), &srvDesc, pTextureView.put()
	));
}
