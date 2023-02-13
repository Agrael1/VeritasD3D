#include <Core/Graphics.h>
#include <Shared/Checks.h>
#include <Shared/Exception.h>
#include <Shared/Profile.h>


ver::IAsyncAction Core::Graphics::InitializeAsync(uint32_t xwidth, uint32_t xheight, bool software)
{
	co_await winrt::resume_background();
	
	VerifySIMDSupport(); //Throws
	width = xwidth; height = xheight;
	EnableDebugLayer();

	if (!factory)
		ver::check_hresult(
			CreateDXGIFactory2(
				ver::debug_mode & DXGI_CREATE_FACTORY_DEBUG,
				__uuidof(IDXGIFactory4), factory.put_void()
			)
		);

	software ?
		GetSoftwareAdapter() :
		GetHardwareAdapter();

	auto cq = [&]()->ver::IAsyncAction {
		ver::scoped_profiler p{ "Command Queue" };
		co_await winrt::resume_background();
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ver::check_hresult(device->CreateCommandQueue(&queueDesc, __uuidof(*command), command.put_void()));
	}();
	auto resources = [&]()->ver::IAsyncAction {
		ver::scoped_profiler p{ "Device Resources" };
		co_await winrt::resume_background();
		// Describe and create a depth stencil view (DSV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ver::check_hresult(device->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(*dsv_heap), dsv_heap.put_void()));
		dsv_heap->SetName(dsv_heap_name);

		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = num_frames;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ver::check_hresult(device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(*rtv_heap), rtv_heap.put_void()));
		rtv_heap_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		rtv_heap->SetName(rtv_heap_name);

		// Create a command allocator for each back buffer that will be rendered to
		for (size_t i = 0; auto& command_allocator : command_allocators)
		{
			ver::check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*command_allocator), command_allocator.put_void()));
			command_allocator->SetName(std::format(L"Command Allocator {}", i++).c_str());
		}

		// Create Command List
		ver::check_hresult(
			device->CreateCommandList(0, 
				D3D12_COMMAND_LIST_TYPE_DIRECT, 
				command_allocators[0].get(), 
				nullptr, __uuidof(*command_list), 
				command_list.put_void()
			)
		);
		command_list->SetName(L"Command List");
		ver::check_hresult(command_list->Close());

		// Create Fence
		ver::check_hresult(device->CreateFence(fence_values[backbuffer_index], D3D12_FENCE_FLAG_NONE, __uuidof(fence), fence.put_void()));
		fence_values[backbuffer_index]++;
		fence->SetName(L"Main Fence");
	}();

	co_await winrt::when_all(resources, cq);
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

void Core::Graphics::VerifySIMDSupport()
{
	if (!DirectX::XMVerifyCPUSupport())
	{
		ver::std_critical("SIMD Intrinsics are not supported");
		throw ver::exception{};
	}
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