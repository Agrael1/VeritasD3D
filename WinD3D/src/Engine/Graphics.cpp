#include <Engine/Graphics.h>
#include <sstream>
#include <bindings/imgui_impl_dx11.h>
#include <bindings/imgui_impl_win32.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/RenderTarget.h>
#include <dxgi1_6.h>
#include <iostream>

namespace dx = DirectX;


constexpr std::array featureLevels
{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1
};

// Graphics
Graphics::Graphics(uint32_t width, uint32_t height, bool software)
	: width(width),
	height(height),
	projection(DirectX::XMMatrixIdentity()),
	camera(DirectX::XMMatrixIdentity())
{
	ver::check_hresult(CreateDXGIFactory1(__uuidof(IDXGIFactory4), factory.put_void()));
	software ?
		GetSoftwareAdapter() :
		GetHardwareAdapter();

	// init imgui d3d impl
	ImGui_ImplDX11_Init(pDevice.get(), pContext.get());
}
Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

bool Graphics::StereoStatus()
{
	return factory->IsWindowedStereoEnabled();
}

void Graphics::GetHardwareAdapter()
{
	winrt::com_ptr<IDXGIAdapter1> adapter;
	auto factory6 = factory.as<IDXGIFactory6>();

	auto queried = [&](uint32_t index) {
		return factory6->EnumAdapterByGpuPreference(index,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			__uuidof(IDXGIAdapter1), adapter.put_void()) >= 0;
	};
	auto fallback = [&](uint32_t index) {
		return factory->EnumAdapters1(index, adapter.put()) >= 0;
	};


	decltype(queried)& x =
		factory6 ? queried : reinterpret_cast<decltype(queried)&>(fallback);

	for (UINT adapterIndex = 0; x(adapterIndex); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		// Don't select the Basic Render Driver adapter.
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		ver::check_graphics(D3D11CreateDevice(
			adapter.get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			0,
			DEBUG_MODE ? D3D11_CREATE_DEVICE_DEBUG : 0,
			featureLevels.data(),
			featureLevels.size(),
			D3D11_SDK_VERSION,
			pDevice.put(),
			nullptr,
			pContext.put()
		));

	}
	if (!pDevice) {
		std::cout << "Getting hardware adapter failed, fallback to WARP";
		GetSoftwareAdapter();
	}
}
void Graphics::GetSoftwareAdapter()
{
	winrt::com_ptr<IDXGIAdapter> adapter;
	ver::check_graphics(factory->EnumWarpAdapter(__uuidof(adapter), adapter.put_void()));
	ver::check_graphics(D3D11CreateDevice(
		adapter.get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		0,
		DEBUG_MODE ? D3D11_CREATE_DEVICE_DEBUG : 0,
		featureLevels.data(),
		featureLevels.size(),
		D3D11_SDK_VERSION,
		pDevice.put(),
		nullptr,
		pContext.put()
	));
}
winrt::IAsyncAction Graphics::CreateSwapChain(HWND wnd)
{
	co_await winrt::resume_background();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = num_frames;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	stereoEnabled = swapChainDesc.Stereo = StereoStatus();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.SampleDesc.Count = 1;

	winrt::com_ptr<IDXGISwapChain1> swapChain;
	ver::check_hresult(factory->CreateSwapChainForHwnd(
		pDevice.get(),
		(HWND)wnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.put()
	));
	ver::check_hresult(swapChain.as(__uuidof(IDXGISwapChain3), pSwap.put_void()));

	// gain access to texture subresource in swap chain (back buffer)
	winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
	ver::check_graphics(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.put_void()));
	pLeftTarget = OutputOnlyRenderTarget::create(*this, pBackBuffer.get());
	pRightTarget = stereoEnabled ? OutputOnlyRenderTarget::create(*this, pBackBuffer.get()) : nullptr;

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
	pLeftTarget->Clear(*this, { r,g,b,1.0 });
	if (pRightTarget)pRightTarget->Clear(*this, { r,g,b,1.0 });
}
void Graphics::EndFrame()
{
	// imgui render
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	winrt::hresult hr = pSwap->Present(1u, 0u);
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
		throw ver::make_error<ver::device_error>({ pDevice->GetDeviceRemovedReason(), infoManager.GetMessages() });
	ver::check_graphics(hr);
	frame_step = timer.stop();
}
void Graphics::DrawIndexed(UINT count) noxnd
{
	pContext->DrawIndexed(count, 0u, 0u);
	ver::check_context();
}
void Graphics::Draw(uint32_t vcount) noxnd
{
	pContext->Draw(vcount, 0u);
	ver::check_context();
}
void Graphics::OnResize(unsigned newwidth, unsigned newheight)
{
	if (pRightTarget)
		pRightTarget->ReleaseBuffer();
	pLeftTarget->ReleaseBuffer();
	ver::check_graphics(pSwap->ResizeBuffers(0, newwidth, newheight, DXGI_FORMAT_UNKNOWN, 0));
	width = newwidth;
	height = newheight;

	// gain access to texture subresource in swap chain (back buffer)
	winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
	ver::check_graphics(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.put_void()));
	pLeftTarget->Reset(*this, pBackBuffer.get());
	if (pRightTarget)pRightTarget->Reset(*this, pBackBuffer.get());//TBD

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

