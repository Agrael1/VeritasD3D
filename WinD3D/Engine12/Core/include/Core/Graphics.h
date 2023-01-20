#pragma once
#include <winrt/base.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Shared/DXGIInfoManager.h>
#include <Shared/Definitions.h>

namespace Core
{
	class Graphics
	{
		inline static constexpr auto num_frames = 2;
		inline static winrt::com_ptr<IDXGIFactory4> factory;
	public:
		Graphics() = default;
		Graphics(uint32_t width, uint32_t height, bool software);
		ver::IAsyncAction InitializeAsync(uint32_t width, uint32_t height, bool software);
	private:
		ver::IAsyncAction CreateSwapChain(void* wnd);
		void Initialize(uint32_t width, uint32_t height, bool software);
		static void EnableDebugLayer();
		void GetHardwareAdapter();
		void GetSoftwareAdapter();
	private:
		ver::DXGIInfoManager info;

		winrt::com_ptr<ID3D12Device> device;
		winrt::com_ptr<ID3D12CommandQueue> command;
		winrt::com_ptr<IDXGISwapChain3> swap;
		winrt::com_ptr<ID3D12DescriptorHeap> rtv_heap;
		winrt::com_ptr<ID3D12CommandAllocator> command_allocator;
		uint32_t width, height;
		uint32_t heap_size;
	};
}