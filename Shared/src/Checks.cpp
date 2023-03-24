#include <Shared/Checks.h>
#include <Shared/DXGIInfoManager.h>
#include <Shared/Log.h>


bool ver::log_dxgi_errors()noexcept
{
	using enum ver::Severity;

	if (!DXGIInfoManager::GetNumMessages())return false;
	auto messages = DXGIInfoManager::GetMessages();
	bool bError = false;
	for (auto& i : messages)
	{
		bError |= i.severity == error || i.severity == critical;
		spd_log(i.severity, i.description);
	}
	return bError;
}

winrt::hresult ver::last_windows_error()noexcept
{
	return winrt::hresult(GetLastError());
}

