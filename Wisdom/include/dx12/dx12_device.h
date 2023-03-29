#pragma once
#include <Shared/Checks.h>
#include <d3d12.h>
#include <d3dx12/d3dx12.h>
#include <dx12/dx12_adapter.h>
#include <dx12/dx12_command_queue.h>

namespace ver
{
	class DX12Device;

	template<>
	class Internal<DX12Device>
	{
		static constexpr inline bool valid = true;
	public:
		template<class Self>
		[[nodiscard]] auto GetDevice(this Self&& s) {
			return s.device;
		}
		template<class Self>
		[[nodiscard]] auto GetTRVDescHeap(this Self&& s) {
			return s.rtv_heap;
		}
	protected:
		winrt::com_ptr<ID3D12Device10> device{};
		winrt::com_ptr<ID3D12DescriptorHeap> rtv_heap{};
	};


	class DX12Device : private Internal<DX12Device>
	{
	public:
		DX12Device() = default;
		DX12Device(DX12Adapter adapter) {
			ver::check_hresult(D3D12CreateDevice(adapter.GetInternal().GetAdapter().get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(*device), device.put_void()));
			ver::check_hresult(CreateDescriptorHeap());
		};
	public:
		bool Initialize(DX12Adapter adapter)noexcept {
			return ver::succeded(D3D12CreateDevice(adapter.GetInternal().GetAdapter().get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(*device), device.put_void())) 
				&& ver::succeded(CreateDescriptorHeap());
		}
		[[nodiscard]]
		auto& GetInternal()const noexcept
		{
			return static_cast<const Internal<DX12Device>&>(*this);
		}
		explicit operator bool()const noexcept
		{
			return device && rtv_heap;
		}
	public:
		[[nodiscard]]
		DX12CommandQueue CreateCommandQueue(QueueOptions options = QueueOptions{})const
		{
			winrt::com_ptr<ID3D12CommandQueue> queue;
			D3D12_COMMAND_QUEUE_DESC desc
			{
				.Type = D3D12_COMMAND_LIST_TYPE(options.type),
				.Priority = int(options.priority),
				.Flags = D3D12_COMMAND_QUEUE_FLAGS(options.flags),
				.NodeMask = options.node_mask
			};
			ver::check_hresult(device->CreateCommandQueue(&desc, __uuidof(*queue), queue.put_void()));
			return { std::move(queue) };
		}
	private:
		winrt::hresult CreateDescriptorHeap()noexcept
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = SwapchainOptions::frame_count,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0 // for now
			};
			return device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(*rtv_heap), rtv_heap.put_void());
		}
	};
}