#pragma once
#include <Shared/Severity.h>
#include <vector>
#include <string>
#include <winrt/base.h>
#include <dxgidebug.h>


struct IDXGIInfoQueue;

namespace ver
{
	struct DXGIMessage
	{
		Severity severity;
		std::string description;
	};

	class DXGIInfoManager
	{
	public:
		DXGIInfoManager();
		~DXGIInfoManager();
		DXGIInfoManager(const DXGIInfoManager&) = delete;
		DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
	public:
		[[nodiscard]] static uint64_t GetNumMessages()noexcept;
		[[nodiscard]] static std::vector<DXGIMessage> GetMessages()noexcept;
	private:
		static winrt::com_ptr<IDXGIInfoQueue> info_queue;
	};
}