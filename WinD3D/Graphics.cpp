#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib,"d3d11.lib")

// Exceptions
Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> messages) noexcept
	:GException(line,file),hr(hr)
{
	// join messages in a single string
	for (const auto& m : messages)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}
std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}
std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, 512u);
	return buf;
}
std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}
const char* Graphics::HrException::what()const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]: 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String]: " << GetErrorString() << std::endl
		<< "[Description]: " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n [Error Info]:\n" << GetErrorInfo() << std::endl << std::endl;
	}

	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
const char* Graphics::HrException::GetType()const noexcept
{
	return "Veritas Graphics Exception";
}

const char* Graphics::DeviceRemovedException::GetType()const noexcept
{
	return "Graphics Device Exception [Device Removed](DXGI_ERROR_DEVICE_REMOVED)";
}

// Graphics
Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC DSwapDesc = {};
	DSwapDesc.BufferDesc.Width = 0;
	DSwapDesc.BufferDesc.Height = 0;
	DSwapDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	DSwapDesc.BufferDesc.RefreshRate.Numerator = 0;
	DSwapDesc.BufferDesc.RefreshRate.Denominator = 0;
	DSwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	DSwapDesc.SampleDesc.Count = 1;
	DSwapDesc.SampleDesc.Quality = 0;

	DSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DSwapDesc.BufferCount = 1;
	DSwapDesc.OutputWindow = hWnd;
	DSwapDesc.Windowed = TRUE;
	DSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	DSwapDesc.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG


	HRESULT hr;

	// create a device with pointers
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DSwapDesc,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	// access to back buffer
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(), nullptr, &pTarget
	));
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
void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r,g,b,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}