#include <Core/Graphics.h>
#include <Shared/Checks.h>
#include <Shared/Log.h>

Core::Graphics::Graphics(uint32_t width, uint32_t height, bool software)
{
	Initialize(width, height, software);
}

void Core::Graphics::Initialize(uint32_t xwidth, uint32_t xheight, bool software)
{
	width = xwidth; height = xheight;
	EnableDebugLayer();

	if (!factory)
		ver::check_hresult(CreateDXGIFactory1(__uuidof(IDXGIFactory4), factory.put_void()));

	software ?
		GetSoftwareAdapter() :
		GetHardwareAdapter();

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ver::check_hresult(device->CreateCommandQueue(&queueDesc, __uuidof(*command), command.put_void()));

	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = num_frames;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ver::check_hresult(device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(*rtv_heap), rtv_heap.put_void()));
	heap_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	ver::check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*command_allocator), command_allocator.put_void()));
}
ver::IAsyncAction Core::Graphics::InitializeAsync(uint32_t xwidth, uint32_t xheight, bool software)
{
	co_await winrt::resume_background();
	width = xwidth; height = xheight;
	EnableDebugLayer();

	if (!factory)
		ver::check_hresult(CreateDXGIFactory1(__uuidof(IDXGIFactory4), factory.put_void()));

	software ?
		GetSoftwareAdapter() :
		GetHardwareAdapter();

	auto c_alloc = [&]()->ver::IAsyncAction {
		co_await winrt::resume_background();
		ver::check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*command_allocator), command_allocator.put_void()));
	}();
	auto cq = [&]()->ver::IAsyncAction {
		co_await winrt::resume_background();
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ver::check_hresult(device->CreateCommandQueue(&queueDesc, __uuidof(*command), command.put_void()));
	}();
	auto heap = [&]()->ver::IAsyncAction {
		co_await winrt::resume_background();
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = num_frames;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ver::check_hresult(device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(*rtv_heap), rtv_heap.put_void()));
		heap_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}();

	co_await winrt::when_all(cq, heap, c_alloc);
}

ver::IAsyncAction Core::Graphics::CreateSwapChain(void* wnd)
{
	co_await winrt::resume_background();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = num_frames;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	winrt::com_ptr<IDXGISwapChain1> swapChain;
	ver::check_hresult(factory->CreateSwapChainForHwnd(
		command.get(),        // Swap chain needs the queue so that it can force a flush on it.
		(HWND)wnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.put()
	));
	ver::check_hresult(swapChain.as(__uuidof(IDXGISwapChain3), swap.put_void()));
}


void Core::Graphics::EnableDebugLayer()
{
	if constexpr (ver::debug_mode)
	{
		winrt::com_ptr<ID3D12Debug> debugController;
		if (ver::check_hresult_nothrow(D3D12GetDebugInterface(__uuidof(ID3D12Debug), debugController.put_void())))
			debugController->EnableDebugLayer();
	}
}
void Core::Graphics::GetSoftwareAdapter()
{
	winrt::com_ptr<IDXGIAdapter> adapter;
	ver::check_hresult(factory->EnumWarpAdapter(__uuidof(adapter), adapter.put_void()));
	ver::check_hresult(D3D12CreateDevice(
		adapter.get(),
		D3D_FEATURE_LEVEL_11_0,
		__uuidof(*device), device.put_void()
	));
}
void Core::Graphics::GetHardwareAdapter()
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

		// Check to see whether the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), device.put_void())))
			return;
	}
	if (!device) {
		ver::std_warn("Getting hardware adapter failed, fallback to WARP");
		GetSoftwareAdapter();
	}
}