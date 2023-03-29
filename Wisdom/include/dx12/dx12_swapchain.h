#pragma once
#include <api/api_swapchain.h>
#include <api/api_internal.h>
#include <dxgi1_6.h>

namespace ver
{
	class DX12SwapChain;

	template<>
	class Internal<DX12SwapChain>
	{
		friend class DX12SwapChain;
		static constexpr inline bool valid = true;
	public:
		Internal() = default;
		Internal(winrt::com_ptr<IDXGISwapChain4> chain)
			:chain(std::move(chain)) {}
	public:
		template<class Self>
		[[nodiscard]] auto GetSwapChain(this Self&& s) {
			return s.chain;
		}
	protected:
		winrt::com_ptr<IDXGISwapChain4> chain{};
	public:
	};

	class DX12SwapChain
	{
	public:
		DX12SwapChain() = default;
		DX12SwapChain(winrt::com_ptr<IDXGISwapChain4> chain) :intern(std::move(chain)) {};
	public:
		uint32_t GetBackBufferIndex()const noexcept
		{
			return intern.chain->GetCurrentBackBufferIndex();
		}
	public:
		[[nodiscard]]
		auto& GetInternal()const noexcept
		{
			return intern;
		}
	private:
		Internal<DX12SwapChain> intern;
	};
}
