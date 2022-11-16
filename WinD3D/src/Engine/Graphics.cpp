#include <Engine/Graphics.h>
#include <sstream>
#include <bindings/imgui_impl_dx11.h>
#include <bindings/imgui_impl_win32.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/RenderTarget.h>

namespace dx = DirectX;

// Graphics
Graphics::Graphics(HWND hWnd, unsigned width, unsigned height)
	: width(width),
	height(height),
	projection(DirectX::XMMatrixIdentity()),
	camera(DirectX::XMMatrixIdentity())
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;


	// create device and front/back buffers, and swap chain and rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		pSwap.put(),
		pDevice.put(),
		nullptr,
		pContext.put()
	));

	// gain access to texture subresource in swap chain (back buffer)
	winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.put_void()));
	pTarget = OutputOnlyRenderTarget::create(*this, pBackBuffer.get());

	// viewport always fullscreen (for now)
	D3D11_VIEWPORT vp{};
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	// init imgui d3d impl
	ImGui_ImplDX11_Init(pDevice.get(), pContext.get());
}
Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
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
	timer.start();
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	pTarget->Clear(*this, { r,g,b,1.0 });
}
void Graphics::EndFrame()
{
	// imgui render
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

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
	frame_step = timer.stop();
}
void Graphics::DrawIndexed(UINT count) noxnd
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}
void Graphics::Draw(uint32_t vcount) noxnd
{
	GFX_THROW_INFO_ONLY(pContext->Draw(vcount, 0u));
}
void Graphics::OnResize(unsigned newwidth, unsigned newheight)
{
	HRESULT hr;
	pTarget->ReleaseBuffer();
	GFX_THROW_INFO(pSwap->ResizeBuffers(0, newwidth, newheight, DXGI_FORMAT_UNKNOWN, 0));
	width = newwidth;
	height = newheight;

	// gain access to texture subresource in swap chain (back buffer)
	winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.put_void()));
	pTarget->Reset(*this, pBackBuffer.get());

	// viewport always fullscreen (for now)
	D3D11_VIEWPORT vp{};
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
}

