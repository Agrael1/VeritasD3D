#pragma once
#include <winrt/base.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Shared/DXGIInfoManager.h>
#include <Shared/Definitions.h>
#include <Shared/Timer.h>

namespace Core
{
	class Graphics
	{
		inline static constexpr auto num_frames = 2;
		inline static constexpr auto rtv_heap_name = L"RenderTargets";
		inline static constexpr auto dsv_heap_name = L"DepthStencil";
		inline static winrt::com_ptr<IDXGIFactory4> factory;
	public:
		Graphics() = default;
		ver::IAsyncAction InitializeAsync(uint32_t width, uint32_t height, bool software);
	private:
		ver::IAsyncAction CreateSwapChain(void* wnd);
		static void EnableDebugLayer();
		void GetHardwareAdapter();
		void GetSoftwareAdapter();
		void VerifySIMDSupport();
	private:
		ver::DXGIInfoManager info;
		ver::timer time;

		winrt::com_ptr<ID3D12Device> device;
		winrt::com_ptr<ID3D12CommandQueue> command;
		winrt::com_ptr<IDXGISwapChain3> swap;
		winrt::com_ptr<ID3D12DescriptorHeap> rtv_heap;
		winrt::com_ptr<ID3D12DescriptorHeap> dsv_heap;
		std::array<winrt::com_ptr<ID3D12CommandAllocator>, num_frames> command_allocators;
		uint32_t width = 0, height = 0;
		uint32_t rtv_heap_size = 0;
		uint32_t dsv_heap_size = 0;
	};
}