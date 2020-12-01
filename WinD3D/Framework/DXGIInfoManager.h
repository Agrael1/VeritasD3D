#pragma once
#include "Framework\WinSetup.h"
#include <wrl.h>
#include "IDXGIInfoManager.h"
#include <DXGIDebug.h>

class DXGIInfoManager : public IDXGIInfoManager
{
public:
	DXGIInfoManager();
	~DXGIInfoManager() = default;
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
public:
	void Set()noexcept override;
	std::vector<std::string> GetMessages()const override;
private:
	unsigned long long next = 0Ui64;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};