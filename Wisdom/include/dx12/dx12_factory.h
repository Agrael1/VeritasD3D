#pragma once
#include <Shared/Checks.h>
#include <api/api_factory.h>
#include <api/api_internal.h>
#include <dx12/dx12_adapter.h>
#include <util/generator.h>

#include <dxgi1_6.h>
#include <d3d12.h>

namespace ver
{
	class DX12Factory;
	inline constexpr DXGI_GPU_PREFERENCE to_dxgi(AdapterPreference pref)
	{
		switch (pref)
		{
		default:
		case ver::AdapterPreference::None:
			return DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_UNSPECIFIED;
		case ver::AdapterPreference::MinConsumption:
			return DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_MINIMUM_POWER;
		case ver::AdapterPreference::Performance:
			return DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		}
	}


	template<>
	class Internal<DX12Factory>
	{
		static constexpr inline bool valid = true;
	public:
		static winrt::com_ptr<IDXGIFactory4> GetFactory() {
			return factory;
		}
	protected:
		static inline winrt::com_ptr<IDXGIFactory4> factory{};
	};


	/// Main Factory class, since we don't need more than one factory it is a static resource
	/// Not thread safe on creation
	class DX12Factory final :
		public APIFactory<DX12Factory>,
		private Internal<DX12Factory>
	{
		static inline constexpr uint32_t debug_flag = ver::debug_mode & DXGI_CREATE_FACTORY_DEBUG;
	public:
		DX12Factory(bool use_preference = true)
		{
			if (factory)
				return;

			EnableDebugLayer();
			ver::check_hresult(CreateDXGIFactory2(debug_flag,
				__uuidof(IDXGIFactory4), factory.put_void()));

			// TODO: consider constexpr
			has_preference = bool(factory.as<IDXGIFactory6>()) && use_preference;
		}
	public:
		[[nodiscard]]
		ver::generator<DX12Adapter> EnumerateAdapters(AdapterPreference preference = AdapterPreference::Performance)const noexcept
		{
			auto gen = has_preference ? AdaptersByGPUPreference(to_dxgi(preference)) : Adapters();
			for (auto&& i : gen)
				co_yield DX12Adapter(i);
		}
	public:
		[[nodiscard]] 
		auto& GetInternal()const noexcept
		{
			return static_cast<const Internal<DX12Factory>&>(*this);
		}
	private:
		void EnableDebugLayer() noexcept
		{
			if constexpr (ver::debug_mode) {
				winrt::com_ptr<ID3D12Debug> debugController;
				if (ver::succeded(D3D12GetDebugInterface(__uuidof(*debugController), debugController.put_void())))
					debugController->EnableDebugLayer();

				if (auto d1 = debugController.as<ID3D12Debug1>())
					d1->SetEnableGPUBasedValidation(true);
			}
		}

		ver::generator<winrt::com_ptr<IDXGIAdapter1>>
			AdaptersByGPUPreference(DXGI_GPU_PREFERENCE preference = DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE)const noexcept
		{
			winrt::com_ptr<IDXGIAdapter1> adapter;
			auto factory6 = factory.as<IDXGIFactory6>();
			uint32_t index = 0;

			while (SUCCEEDED(factory6->EnumAdapterByGpuPreference(index++,
				preference,
				__uuidof(IDXGIAdapter1), adapter.put_void())))
			{
				co_yield std::move(adapter);
			}
		}
		ver::generator<winrt::com_ptr<IDXGIAdapter1>>
			Adapters()const noexcept
		{
			winrt::com_ptr<IDXGIAdapter1> adapter;
			uint32_t index = 0;

			while (SUCCEEDED(factory->EnumAdapters1(index++, adapter.put())))
				co_yield std::move(adapter);
		}

	private:
		static inline bool has_preference = false;
	};
}