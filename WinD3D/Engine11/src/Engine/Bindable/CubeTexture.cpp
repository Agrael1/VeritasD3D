#include <Engine/Bindable/CubeTexture.h>
#include <Engine/Loading/Image.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Bindable/DepthStencil.h>

constexpr uint32_t DefaultDifTexture = 0x0;

using namespace ver;

ver::CubeTexture::CubeTexture(Graphics& gfx, std::filesystem::path path, uint32_t slot)
	:slot(slot), path(std::move(path))
{
	Initialize(gfx);
}

winrt::IAsyncAction ver::CubeTexture::InitializeAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot)
{
	co_await winrt::resume_background();
	this->path = std::move(path);
	this->slot = slot;
	Initialize(gfx);
}

void ver::CubeTexture::Initialize(Graphics& gfx)
{
	DirectX::ScratchImage image;
	DirectX::TexMetadata info;
	DirectX::LoadFromDDSFile(path.native().c_str(), DirectX::DDS_FLAGS_ALLOW_LARGE_FILES, &info, image);

	DirectX::CreateShaderResourceView(GetDevice(gfx),
		image.GetImages(), image.GetImageCount(), info, pCubeTextureView.put());
}

void ver::CubeTexture::Bind(Graphics& gfx) noexcept(!DEBUG_MODE)
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, array_view(pCubeTextureView)));
}


DepthCubeTexture::DepthCubeTexture(Graphics& gfx, UINT size, UINT slot)
	:
	slot(slot)
{
	INFOMAN(gfx);

	// texture descriptor
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	// create the texture resource
	winrt::com_ptr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc, nullptr, pTexture.put()
	));
	
	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.get(), &srvDesc, pTextureView.put()
	));

	// make depth buffer resources for capturing shadow map
	for (UINT face = 0; face < 6; face++)
	{
		depthBuffers.push_back(std::make_shared<OutputOnlyDepthStencil>(gfx, pTexture, face));
	}
}

std::shared_ptr<OutputOnlyDepthStencil> ver::DepthCubeTexture::GetDepthBuffer(size_t index) const
{
	return depthBuffers[index];
}

void DepthCubeTexture::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, array_view(pTextureView)));
}
