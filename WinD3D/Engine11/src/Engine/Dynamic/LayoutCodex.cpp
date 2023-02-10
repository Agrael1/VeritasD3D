#include <Engine/Dynamic/LayoutCodex.h>

namespace DC
{
	CookedLayout LayoutCodex::Resolve(RawLayout&& layout) noxnd
	{
		auto sig = layout.GetSignature();
		auto& map = Get().map;
		const auto i = map.find(sig);
		// idential layout already exists
		if (i != map.end())
		{
			// input layout is expected to be cleared after Resolve
			// so just throw away the layout tree
			layout.ClearRoot();
			return { i->second };
		}
		// otherwise add layout root element to map
		auto result = map.insert({ std::move(sig),layout.DeliverRoot() });
		// return layout with additional reference to root
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}
