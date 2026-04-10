#pragma once
#include "Bindable.h"
#include "BufferResource.h"
#include <d3d11.h>

class Graphics;
class RenderTarget;

namespace ver
{

	class DepthStencil : public Bindable, public BufferResource
	{
		friend RenderTarget;
		friend class RenderTargetArray;
	public:
		enum class Usage
		{
			DepthStencil,
			ShadowDepth,
		};
	public:
		void BindAsBuffer(Graphics& gfx) noxnd override;
		void BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noxnd override;
		void BindAsBuffer(Graphics& gfx, RenderTarget* rt) noxnd;
		void Clear(Graphics& gfx) noxnd override;
		void Dumpy(Graphics& gfx, const std::string& path) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	private:
		std::pair<winrt::com_ptr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> MakeStaging(Graphics& gfx) const;
	protected:
		DepthStencil(Graphics& gfx, winrt::com_ptr<ID3D11Texture2D> pTexture, UINT face);
		DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, Usage usage);
		winrt::com_ptr<ID3D11DepthStencilView> pDepthStencilView;
		unsigned int width;
		unsigned int height;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(Graphics& gfx, UINT slot = 0u, Usage usage = Usage::DepthStencil);
		ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil);
		void Bind(Graphics& gfx) noxnd override;
		ID3D11ShaderResourceView* GetSRV()const noexcept { return pShaderResourceView.get(); }
	private:
		UINT slot;
		winrt::com_ptr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil(Graphics& gfx, winrt::com_ptr<ID3D11Texture2D> pTexture, UINT face);
		OutputOnlyDepthStencil(Graphics& gfx);
		OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height);
		void Bind(Graphics& gfx) noxnd override;
	};
}