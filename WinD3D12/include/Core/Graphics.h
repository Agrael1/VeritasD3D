#pragma once
#include <factory.h>

namespace WinD3D12
{
	class Graphics
	{
	public:
		ver::IAsyncAction InitializeAsync();
		ver::IAsyncAction InitializeSwapChainAsync(ver::SwapchainOptions options, ver::SurfaceParameters surface);
	private:
		ver::Factory factory;
		ver::Device device;
		ver::CommandQueue queue;
		ver::SwapChain swap;
	};
}