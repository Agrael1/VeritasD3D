#pragma once
#ifndef MODULE
#include <Shared/Severity.h>
#include <vector>
#include <string>
#include <winrt/base.h>
#endif

struct IDXGIInfoQueue;

SHARED_EXPORT namespace ver
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
		static uint64_t GetNumMessages();
		static std::vector<DXGIMessage> GetMessages();
	private:
		static winrt::com_ptr<IDXGIInfoQueue> info_queue;
	};
}