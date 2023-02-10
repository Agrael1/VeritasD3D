#pragma once
#include <Engine/Dynamic/DynamicConstant.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace DC
{
	class LayoutCodex
	{
	public:
		static DC::CookedLayout Resolve(DC::RawLayout&& layout) noxnd;
	private:
		static LayoutCodex& Get() noexcept;
	private:
		std::unordered_map<std::string, std::shared_ptr<DC::LayoutElement>> map;
	};
}