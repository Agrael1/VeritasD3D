#pragma once
#include <Engine/Bindable/Bindable.h>
#include <Engine/Bindable/BufferResource.h>
#include <span>

namespace ver
{
	class DepthStencil;
}

class IRenderTarget : public Bindable, public BufferResource
{
public:
	virtual void BindAsTarget(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd = 0;
};

class RenderTarget : public IRenderTarget
{
public:
	void BindAsBuffer(Graphics& gfx) noxnd override;
	void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd override;
	void BindAsBuffer(Graphics& gfx, ver::DepthStencil* depthStencil) noxnd;
	void Clear(Graphics& gfx) noxnd override;
	void Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
private:
	void BindAsTarget(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd override;
protected:
	RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture, uint32_t array_slice);
	RenderTarget(Graphics& gfx, UINT width, UINT height);
protected:
	UINT width;
	UINT height;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
};

class ShaderInputRenderTarget : public RenderTarget
{
public:
	ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot);
	void Bind(Graphics& gfx) noxnd override;
	void BindTo(Graphics& gfx, uint32_t slot) noxnd;
private:
	UINT slot;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
};

// RT for Graphics to create RenderTarget for the back buffer
class OutputOnlyRenderTarget : public RenderTarget
{
	friend Graphics;
protected:
	static std::shared_ptr<OutputOnlyRenderTarget> create(Graphics& gfx, ID3D11Texture2D* pTexture, uint32_t array_slice = 0)
	{
		return std::shared_ptr<OutputOnlyRenderTarget>{ new OutputOnlyRenderTarget(gfx, pTexture, array_slice) };
	}
public:
	void Bind(Graphics& gfx) noxnd override;
	void ReleaseBuffer();
	void Reset(Graphics& gfx, ID3D11Texture2D* pTexture);
private:
	OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture, uint32_t array_slice);
private:
	uint32_t array_slice = 0;
};

class RenderTargetArray : public IRenderTarget
{

public:
	RenderTargetArray() = default;
	winrt::IAsyncAction InitializeAsync(Graphics& gfx, uint32_t xwidth, uint32_t xheight, uint32_t slot);
	void BindAsBuffer(Graphics& gfx) noxnd override;
	void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd override;
	void BindAsBuffer(Graphics& gfx, ver::DepthStencil* depthStencil) noxnd;

	void Clear(Graphics& gfx) noxnd override;
	void Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd;
public:
	uint32_t GetWidth() const noexcept { return width; }
	uint32_t GetHeight() const noexcept { return height; }
	void Bind(Graphics& gfx) noxnd override;
	ID3D11RenderTargetView* GetRTV(size_t at) { return targets.at(at).get(); }
	ID3D11ShaderResourceView* GetSRV(size_t at) { return resource_views.at(at).get(); }
private:
	void BindAsTarget(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd override;
protected:
	uint32_t slot = 0u;
	uint32_t width = 0u;
	uint32_t height = 0u;
	std::array<winrt::com_ptr<ID3D11RenderTargetView>, 5> targets;
	std::array<winrt::com_ptr<ID3D11ShaderResourceView>, 5> resource_views;
};