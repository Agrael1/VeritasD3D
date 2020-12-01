#include "pch.h"
#include "Graphics.h"
#include "GraphicsThrows.h"
#include <Engine/Architecture/RenderTarget.h>

namespace dx = DirectX;

Graphics::Graphics(unsigned width, unsigned height, IUnknown* pDrawSwapchain)
	: width(width),
	height(height),
	projection(DirectX::XMMatrixIdentity()),
	camera(DirectX::XMMatrixIdentity())
{
	// for checking results of d3d functions
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC1 sd = {};
	sd.Width = width;
	sd.Height = height;
	sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Stereo = false;
	sd.SampleDesc.Count = 1;                          // don't use multi-sampling
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // we recommend using this swap effect for all applications
	sd.Flags = 0;

	UINT DeviceCreateFlags = 0u;
#ifndef NDEBUG
	DeviceCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	GFX_THROW_INFO(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		DeviceCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		pDevice.put(),
		nullptr,
		pContext.put()
	));

	// get the DXGI adapter
	winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
		pDevice.as<IDXGIDevice>()->GetAdapter(dxgiAdapter.put());

	// get the DXGI factory
	winrt::com_ptr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), dxgiFactory.put_void());

	// create swap chain by calling CreateSwapChainForComposition
	dxgiFactory->CreateSwapChainForComposition(
		pDevice.get(),
		&sd,
		nullptr,        // allow on any display 
		pSwap.put()
	);

	winrt::com_ptr<ISwapChainPanelNative> swapChainNative;
	pDrawSwapchain->QueryInterface(__uuidof(ISwapChainPanelNative), swapChainNative.put_void());

	swapChainNative->SetSwapChain(pSwap.get());

	// gain access to texture subresource in swap chain (back buffer)
	winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.put_void()));
	pTarget = std::shared_ptr<RenderTarget>{ new OutputOnlyRenderTarget(*this, pBackBuffer.get()) };

	// viewport always fullscreen (for now)
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
}
Graphics::~Graphics()
{
}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}
void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}
bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

void Graphics::BeginFrame(float r, float g, float b) noexcept
{

}
void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG

	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}
DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}
void Graphics::SetCamera(DirectX::XMMATRIX Camera)noexcept
{
	camera = Camera;
}
void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}
DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}
void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}
UINT Graphics::GetWidth() const noexcept
{
	return width;
}
UINT Graphics::GetHeight() const noexcept
{
	return height;
}
std::shared_ptr<RenderTarget> Graphics::GetTarget()
{
	return pTarget;
}
