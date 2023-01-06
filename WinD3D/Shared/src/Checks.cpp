#include <Shared/Checks.h>
#include <Shared/DXGIInfoManager.h>
#include <Shared/Log.h>
#include <Shared/Exception.h>

void ver::check_static(bool throw_error, std::source_location sl)
{
	using enum ver::Severity;

	if (!DXGIInfoManager::GetNumMessages())return;
	auto messages = DXGIInfoManager::GetMessages();
	bool bError = false;
	for (auto& i : messages)
	{
		bError |= i.severity == error || i.severity == critical;
		std_log(i.severity, i.description);
	}
	if (bError && throw_error) throw ver::exception{ sl };
}