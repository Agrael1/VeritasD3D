#pragma once
#include <vector>
#include <string>

struct IDXGIInfoQueue;

namespace ver
{
	class DXGIInfoManager
	{
	public:
		DXGIInfoManager();
		~DXGIInfoManager();
		DXGIInfoManager(const DXGIInfoManager&) = delete;
		DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
	public:
		static uint64_t GetNumMessages();
		static std::vector<std::string> GetMessages();
	private:
		static winrt::com_ptr<IDXGIInfoQueue> info_queue;
	};
}