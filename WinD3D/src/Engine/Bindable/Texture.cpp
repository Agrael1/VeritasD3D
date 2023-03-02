#include <Engine/Bindable/Texture.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Loading/Image.h>
#include <Shared/Checks.h>

using namespace ver;

constexpr uint32_t DefaultDifTexture = 0xffffffff;
constexpr uint32_t DefaultNrmTexture = 0xff8080ff;
constexpr uint32_t DefaultSpcTexture = 0xffffffff;


Texture::Texture(Graphics& gfx, std::filesystem::path path, uint32_t slot, bool test)
	:slot(slot), path(std::move(path)), test(test)
{
	Initialize(gfx);
}

ver::IAsyncAction Texture::InitializeAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot, bool test)
{
	co_await winrt::resume_background();
	this->test = test;
	this->path = std::move(path);
	this->slot = slot;
	Initialize(gfx);
}
void Texture::Initialize(Graphics& gfx)
{
	if (path.extension() == u".dds")
	{
		DirectX::ScratchImage image;
		DirectX::TexMetadata info;
		DirectX::LoadFromDDSFile(path.native().c_str(), DirectX::DDS_FLAGS_ALLOW_LARGE_FILES, &info, image);

		DirectX::CreateShaderResourceView(GetDevice(gfx),
			image.GetImages(), count = image.GetImageCount(), info, pTextureView.put());

		width = info.width;
		height = info.height;
		return;
	}

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
	

	if (!test)
	{
		(GetContext(gfx)->PSSetShaderResources(slot, 1u, array_view(pTextureView)));
	}
	else
	{
		(GetContext(gfx)->DSSetShaderResources(slot, 1u, array_view(pTextureView)));
	}
	ver::check_context();
}
void Texture::BindTo(Graphics& gfx, uint32_t xslot) noxnd
{
	(GetContext(gfx)->PSSetShaderResources(slot = xslot, 1u, array_view(pTextureView)));	
	ver::check_context();
}
std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, std::filesystem::path path, uint32_t slot, bool test)
{
	return ver::Codex::Resolve<Texture>(gfx, std::move(path), slot, test);
}
concurrency::task<std::shared_ptr<Texture>>
Texture::ResolveAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot, bool test)
{
	return ver::Codex::ResolveAsync<Texture>(gfx, std::move(path), slot, test);
}


std::string Texture::GenerateUID(const std::filesystem::path& path, uint32_t slot, bool test)
{
	return std::format("{}#{}#{}", typeid(Texture).name(), path.string(), std::to_string(slot));
}
std::string Texture::GetUID() const noexcept
{
	return GenerateUID(path, slot, test);
}

void Texture::ResolveToDefault(Graphics& gfx)
{
	
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

	ver::check_hresult(GetDevice(gfx)->CreateTexture2D(
		&texDesc,
		&sd,
		pTexture.put()));

	// create a resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ver::check_hresult(GetDevice(gfx)->CreateShaderResourceView
	(
		pTexture.get(), &srvDesc, pTextureView.put()
	));
}

ver::StagingTexture::StagingTexture(Graphics& gfx, DXGI_FORMAT format, uint32_t width, uint32_t height, uint32_t cpu_flags)
{
	//create texture resource
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = cpu_flags;
	texDesc.MiscFlags = 0;


	ver::check_hresult(GetDevice(gfx)->CreateTexture2D(
		&texDesc,
		nullptr,
		pTexture.put()));
}