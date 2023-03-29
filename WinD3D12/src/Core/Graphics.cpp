#include <Core/Graphics.h>
#include <Shared/Profile.h>


ver::IAsyncAction WinD3D12::Graphics::InitializeAsync()
{
	co_await winrt::resume_background();
	ver::scoped_profiler p;
	for (auto&& a : factory.EnumerateAdapters(ver::AdapterPreference::Performance))
	{
		if (a.GetDesc().IsSoftware()) 
			ver::spd_warn("Loading WARP adapter");
		if (device.Initialize(a))break;
	}

	queue = device.CreateCommandQueue();
}

ver::IAsyncAction WinD3D12::Graphics::InitializeSwapChainAsync(ver::SwapchainOptions options, ver::SurfaceParameters surface)
{
	co_await winrt::resume_background();
	ver::scoped_profiler p;
	swap = factory.CreateSwapchain(options, surface, queue);
}
