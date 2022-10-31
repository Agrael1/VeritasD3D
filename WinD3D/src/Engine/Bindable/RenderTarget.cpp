#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <array>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

namespace wrl = Microsoft::WRL;


RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height)
	:
	width(width),
	height(height)
{
	INFOMAN(gfx);

	// create texture resource
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // never do we not want to bind offscreen RTs as inputs
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc, nullptr, &pTexture
	));

	// create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(
		pTexture.Get(), &rtvDesc, &pTargetView
	));
}

RenderTarget::RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture)
{
	INFOMAN(gfx);

	// get information from texture about dimensions
	D3D11_TEXTURE2D_DESC textureDesc;
	pTexture->GetDesc(&textureDesc);
	width = textureDesc.Width;
	height = textureDesc.Height;

	// create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(
		pTexture, &rtvDesc, &pTargetView
	));
}

void RenderTarget::BindAsBuffer(Graphics& gfx) noxnd
{
	ID3D11DepthStencilView* const null = nullptr;
	BindAsTarget(gfx, null);
}

void RenderTarget::BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd
{
	assert(dynamic_cast<DepthStencil*>(depthStencil) != nullptr);
	BindAsBuffer(gfx, static_cast<DepthStencil*>(depthStencil));
}

void RenderTarget::BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil) noxnd
{
	BindAsTarget(gfx, depthStencil ? depthStencil->pDepthStencilView.Get() : nullptr);
}

void RenderTarget::BindAsTarget(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd
{
	INFOMAN_NOHR(gfx);
	GetContext(gfx)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), pDepthStencilView);

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetViewports(1u, &vp));
}

void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->ClearRenderTargetView(pTargetView.Get(), color.data()));
}

void RenderTarget::Clear(Graphics& gfx) noxnd
{
	Clear(gfx, { 0.0f,0.0f,0.0f,0.0f });
}

UINT RenderTarget::GetWidth() const noexcept
{
	return width;
}
UINT RenderTarget::GetHeight() const noexcept
{
	return height;
}


ShaderInputRenderTarget::ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot)
	:
	RenderTarget(gfx, width, height),
	slot(slot)
{
	INFOMAN(gfx);

	wrl::ComPtr<ID3D11Resource> pRes;
	pTargetView->GetResource(&pRes);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pRes.Get(), &srvDesc, &pShaderResourceView
	));
}

void ShaderInputRenderTarget::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1, pShaderResourceView.GetAddressOf()));
}


void OutputOnlyRenderTarget::Bind(Graphics& gfx) noxnd
{
	assert("Cannot bind OuputOnlyRenderTarget as shader input" && false);
}

OutputOnlyRenderTarget::OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture)
	:RenderTarget(gfx, pTexture)
{}


winrt::com_ptr<ID3D11RenderTargetView> GetRTV(ID3D11Device* dev, uint32_t width, uint32_t height)
{
	winrt::com_ptr<ID3D11RenderTargetView> pTargetView;
	winrt::com_ptr<ID3D11Texture2D> pTexture;

	// create texture resource
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // never do we not want to bind offscreen RTs as inputs
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	winrt::check_hresult(dev->CreateTexture2D(
		&textureDesc, nullptr, pTexture.put()
	));

	// create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	winrt::check_hresult(dev->CreateRenderTargetView(
		pTexture.get(), &rtvDesc, pTargetView.put()
	));
	return pTargetView;
}

winrt::com_ptr<ID3D11ShaderResourceView> GetSRV(ID3D11Device* dev, ID3D11RenderTargetView* rtv)
{
	winrt::com_ptr<ID3D11ShaderResourceView> pShaderResourceView;
	winrt::com_ptr<ID3D11Resource> pRes;
	rtv->GetResource(pRes.put());

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	winrt::check_hresult(dev->CreateShaderResourceView(
		pRes.get(), &srvDesc, pShaderResourceView.put()
	));
	return pShaderResourceView;
}

void RenderTargetArray::BindAsBuffer(Graphics& gfx) noxnd
{
	ID3D11DepthStencilView* null = nullptr;
	BindAsTarget(gfx, null);
}

void RenderTargetArray::BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd
{
	assert(dynamic_cast<DepthStencil*>(depthStencil) != nullptr);
	BindAsBuffer(gfx, static_cast<DepthStencil*>(depthStencil));
}

void RenderTargetArray::BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil) noxnd
{
	BindAsTarget(gfx, depthStencil ? depthStencil->pDepthStencilView.Get() : nullptr);
}

void RenderTargetArray::Clear(Graphics& gfx) noxnd
{
	Clear(gfx, { 0,0,0,0 });
}

void RenderTargetArray::Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd
{
	INFOMAN_NOHR(gfx);
	for (auto& i : targets)
	{
		GFX_THROW_INFO_ONLY(GetContext(gfx)->ClearRenderTargetView(i.get(), color.data()));
	}
}

winrt::IAsyncAction RenderTargetArray::InitializeAsync(Graphics& gfx, uint32_t width, uint32_t height, uint32_t slot)
{
	co_await winrt::resume_background();
	for (auto& i : targets)
	{
		i = GetRTV(GetDevice(gfx), width, height);
	}
	for (size_t i = 0; i < resource_views.size(); i++)
	{
		resource_views[i] = GetSRV(GetDevice(gfx), targets[i].get());
	}
	this->slot = slot;
	this->width = width;
	this->height = height;
}

void RenderTargetArray::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, uint32_t(resource_views.size()), (ID3D11ShaderResourceView**)&resource_views));
}

void RenderTargetArray::BindAsTarget(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd
{
	INFOMAN_NOHR(gfx);
	GetContext(gfx)->OMSetRenderTargets(uint32_t(targets.size()), reinterpret_cast<ID3D11RenderTargetView* const*>(targets.data()), pDepthStencilView);

	// configure viewport
	D3D11_VIEWPORT vp{};
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetViewports(1u, &vp));
}
