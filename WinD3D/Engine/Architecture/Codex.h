#pragma once
#include <unordered_map>
#include <memory>
#include <type_traits>
#include "Bindable.h"

class Codex
{
public:
	template<class T, typename ...Params> 
	static std::shared_ptr<T> Resolve(Graphics& gfx, Params&& ...p)noxnd
	{
		static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
		return Get()._Resolve<T>(gfx, std::forward<Params>(p)...);
	}
	static void Trim()noxnd
	{
		Get()._Trim();
	}
private:
	template<class T, typename ...Params>
	std::shared_ptr<T> _Resolve(Graphics& gfx, Params&& ...p)noxnd
	{
		const auto key = T::GenerateUID(std::forward<Params>(p)...);
		if(binds.find(key) == binds.end())
			binds.try_emplace(key, std::make_shared<T>(gfx, std::forward<Params>(p)...));
		return std::static_pointer_cast<T>(binds.at(key));
	}
	void _Trim()noxnd
	{
		std::vector<const std::string*> todelete;
		todelete.reserve(binds.size());

		for (auto& bind : binds)
		{
			if (bind.second.use_count() == 1)todelete.push_back(&bind.first);
		}
		for (auto& bind : todelete)
			binds.erase(*bind);
	}
	static Codex& Get()
	{
		static Codex codex;
		return codex;
	}
private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
};