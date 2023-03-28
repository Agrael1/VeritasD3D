#pragma once
#include <Shared/Checks.h>
#include <d3d12.h>
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
	protected:
		winrt::com_ptr<ID3D12Device10> device{};
	};


	class DX12Device : private Internal<DX12Device>
	{
	public:
		DX12Device() = default;
		DX12Device(DX12Adapter adapter) {
			ver::check_hresult(D3D12CreateDevice(adapter.GetInternal().GetAdapter().get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(*device), device.put_void()));
		};
	public:
		bool Initialize(DX12Adapter adapter)noexcept {
			return ver::succeded(D3D12CreateDevice(adapter.GetInternal().GetAdapter().get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(*device), device.put_void()));
		}
		[[nodiscard]]
		auto& GetInternal()const noexcept
		{
			return static_cast<const Internal<DX12Device>&>(*this);
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
	};
}