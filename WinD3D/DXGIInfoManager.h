#pragma once
#include <Framework\WinSetup.h>
#include <wrl.h>
#include <vector>
#include <DXGIDebug.h>

class DXGIInfoManager
{
public:
	DXGIInfoManager();
	~DXGIInfoManager() = default;
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
public:
	void Set()noexcept;
	std::vector<std::string> GetMessages()const;
private:
	unsigned long long next = 0Ui64;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};