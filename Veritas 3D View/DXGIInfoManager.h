#pragma once
#include <DXGIDebug.h>

class DXGIInfoManager
{
public:
	DXGIInfoManager();
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
public:
	void Set()noexcept;
	winrt::hstring GetMessages()const;
private:
	unsigned long long next = 0Ui64;
	winrt::com_ptr<IDXGIInfoQueue> pDxgiInfoQueue;
};

