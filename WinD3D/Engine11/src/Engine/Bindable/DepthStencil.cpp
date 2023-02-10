#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/RenderTarget.h>
#include <stdexcept>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Graphics.h>


namespace ver
{
	DXGI_FORMAT MapUsageTypeless(DepthStencil::Usage usage)
	{
		switch (usage)
		{
		case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		}
		throw std::runtime_error{ "Base usage for Typeless format map in DepthStencil." };
	}

	DXGI_FORMAT MapUsageTyped(DepthStencil::Usage usage)
	{
		switch (usage)
		{
		case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		}
		throw std::runtime_error{ "Base usage for Typed format map in DepthStencil." };
	}

	DXGI_FORMAT MapUsageColored(DepthStencil::Usage usage)
	{
		switch (usage)
		{
		case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		}
		throw std::runtime_error{ "Base usage for Colored format map in DepthStencil." };
	}

	DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, Usage usage)
		:
		width(width),
		height(height)
	{
		INFOMAN(gfx);

		// create depth stensil texture
		winrt::com_ptr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = MapUsageTypeless(usage);
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, pDepthStencil.put()));

		// create target view of depth stensil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = MapUsageTyped(usage);
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;
		GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(
			pDepthStencil.get(), &descView, pDepthStencilView.put()
		));
	}

	DepthStencil::DepthStencil(Graphics& gfx, winrt::com_ptr<ID3D11Texture2D> pTexture, UINT face)
	{
		INFOMAN(gfx);

		D3D11_TEXTURE2D_DESC descTex = {};
		pTexture->GetDesc(&descTex);
		width = descTex.Width;
		height = descTex.Height;

		// create target view of depth stensil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.ArraySize = 1;
		descView.Texture2DArray.FirstArraySlice = face;
		GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(
			pTexture.get(), &descView, pDepthStencilView.put()
		));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(0, nullptr, pDepthStencilView.get()));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noxnd
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(gfx, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, RenderTarget* rt) noxnd
	{
		rt->BindAsBuffer(gfx, this);
	}

	void DepthStencil::Clear(Graphics& gfx) noxnd
	{
		GetContext(gfx)->ClearDepthStencilView(pDepthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	std::pair<winrt::com_ptr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> DepthStencil::MakeStaging(Graphics& gfx) const
	{
		INFOMAN(gfx);

		// get info about the stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC srcViewDesc{};
		pDepthStencilView->GetDesc(&srcViewDesc);
		// creating a temp texture compatible with the source, but with CPU read access
		winrt::com_ptr<ID3D11Resource> pResSource;
		pDepthStencilView->GetResource(pResSource.put());
		auto pTexSource = pResSource.as<ID3D11Texture2D>();
		D3D11_TEXTURE2D_DESC srcTextureDesc{};
		pTexSource->GetDesc(&srcTextureDesc);
		D3D11_TEXTURE2D_DESC tmpTextureDesc = srcTextureDesc;
		tmpTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tmpTextureDesc.Usage = D3D11_USAGE_STAGING;
		tmpTextureDesc.BindFlags = 0;
		tmpTextureDesc.MiscFlags = 0;
		tmpTextureDesc.ArraySize = 1;
		winrt::com_ptr<ID3D11Texture2D> pTexTemp;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&tmpTextureDesc, nullptr, pTexTemp.put()
		));

		// copy texture contents
		if (srcViewDesc.ViewDimension == D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
		{
			// source is actually inside a cubemap texture, use view info to find the correct slice and copy subresource
			GFX_THROW_INFO_ONLY(GetContext(gfx)->CopySubresourceRegion(pTexTemp.get(), 0, 0, 0, 0, pTexSource.get(), srcViewDesc.Texture2DArray.FirstArraySlice, nullptr));
		}
		else
		{
			GFX_THROW_INFO_ONLY(GetContext(gfx)->CopyResource(pTexTemp.get(), pTexSource.get()));
		}

		return { std::move(pTexTemp),srcTextureDesc };
	}

	unsigned int DepthStencil::GetWidth() const
	{
		return width;
	}

	unsigned int DepthStencil::GetHeight() const
	{
		return height;
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT slot, Usage usage)
		:
		ShaderInputDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight(), slot, usage)
	{}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage)
		:
		DepthStencil(gfx, width, height, true, usage),
		slot(slot)
	{
		INFOMAN(gfx);

		winrt::com_ptr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource(pRes.put());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = MapUsageColored(usage);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pRes.get(), &srvDesc, pShaderResourceView.put()
		));
	}

	void ShaderInputDepthStencil::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, array_view(pShaderResourceView)));
	}



	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, winrt::com_ptr<ID3D11Texture2D> pTexture, UINT face)
		:
		DepthStencil(gfx, std::move(pTexture), face)
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx)
		:
		OutputOnlyDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height)
		:
		DepthStencil(gfx, width, height, false, Usage::DepthStencil)
	{}

	void OutputOnlyDepthStencil::Bind(Graphics& gfx) noxnd
	{
		assert("OutputOnlyDepthStencil cannot be bound as shader input" && false);
	}
}