#pragma once
#include <api/api_internal.h>
#include <util/flags.h>
#include <string>
#include <array>
#include <Shared/Log.h>

namespace ver
{
	enum class AdapterFlags : uint32_t
	{
		None = 0,
		Remote = 1,
		Software = 2,
		ACGCompatible = 4,

		//mostly unused 
		SUPPORT_MONITORED_FENCES = 8,
		SUPPORT_NON_MONITORED_FENCES = 0x10,
		KEYED_MUTEX_CONFORMANCE = 0x20,
		FORCE_DWORD = 0xffffffff
	};


	struct AdapterDesc
	{
		static inline constexpr uint32_t uid_length = 8;

		std::wstring description;
		uint32_t vendor_id;
		uint32_t device_id;
		uint32_t subsys_id;
		uint32_t revision;

		size_t dedicated_video_memory;
		size_t dedicated_system_memory;
		size_t shared_system_memory;
		std::array<uint8_t, uid_length> adapter_id;

		AdapterFlags flags = AdapterFlags::None;

		bool IsSoftware()const noexcept{
			using namespace river::flags;
			return flags & AdapterFlags::Software;
		}
	};

	inline consteval auto func(std::source_location sl = std::source_location::current())
	{
		return sl.function_name();
	}

	template<class Impl>
	class APIAdapter : public APIBase<Impl>
	{
		using APIBase<Impl>::impl;
	public:
		AdapterDesc GetDesc()const noexcept
		{
			return impl().GetDesc();
		}
	};
}