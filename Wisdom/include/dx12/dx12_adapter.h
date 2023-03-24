#pragma once
#include <api/api_internal.h>
#include <api/api_adapter.h>

#include <winrt/base.h>
#include <dxgi1_6.h>

namespace ver
{
	class DX12Adapter;

	template<>
	class Internal<DX12Adapter>
	{
		static constexpr inline bool valid = true;
	protected: 
		explicit Internal(winrt::com_ptr<IDXGIAdapter1> adapter)
			:adapter(std::move(adapter)) {}
	public:
		template<class Self>
		auto GetAdapter(this Self&& s) {
			return s.adapter;
		}
	protected:
		winrt::com_ptr<IDXGIAdapter1> adapter;
	};


	class DX12Adapter final : public APIAdapter<DX12Adapter>,
		private Internal<DX12Adapter>
	{
		using intern = Internal<DX12Adapter>;
	public:
		explicit DX12Adapter(winrt::com_ptr<IDXGIAdapter1> adapter)
			:intern(std::move(adapter))
		{}
	public:
		AdapterDesc GetDesc()const noexcept
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			return AdapterDesc{
				.description{desc.Description},
				.vendor_id = desc.VendorId,
				.device_id = desc.DeviceId,
				.subsys_id = desc.SubSysId,
				.revision = desc.Revision,

				.dedicated_video_memory = desc.DedicatedVideoMemory,
				.dedicated_system_memory = desc.DedicatedSystemMemory,
				.shared_system_memory = desc.SharedSystemMemory,
				.adapter_id{reinterpret_cast<std::array<uint8_t, ver::AdapterDesc::uid_length>&>(desc.AdapterLuid)},
				.flags = AdapterFlags(desc.Flags)
			};
		}
	public:
		auto& GetInternal()const
		{
			return static_cast<const Internal<DX12Adapter>&>(*this);
		}
	};
}