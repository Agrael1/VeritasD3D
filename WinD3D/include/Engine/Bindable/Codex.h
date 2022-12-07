#pragma once
#include <Engine/Bindable/Bindable.h>
#include <Engine/Bindable/AsyncResource.h>
#include <concurrent_unordered_map.h>
#include <pplawait.h>
#include <memory>
#include <semaphore>


namespace ver
{
	class scoped_semaphore
	{
	public:
		scoped_semaphore(std::binary_semaphore& a) :a(a) { a.acquire(); }
		~scoped_semaphore() { a.release(); }
	private:
		std::binary_semaphore& a;
	};


	class Codex
	{
	public:
		template<class T, typename ...Params> requires std::derived_from<T, Bindable>
		static std::shared_ptr<T> Resolve(Graphics& gfx, Params&& ...p)noxnd
		{
			return instance()._Resolve<T>(gfx, std::forward<Params>(p)...);
		}
		template<class T, typename ...Params> requires std::derived_from<T, Bindable>
		static concurrency::task <std::shared_ptr<T>> ResolveAsync(Graphics& gfx, Params&& ...p)noxnd
		{
			return instance()._ResolveAsync<T>(gfx, std::forward<Params>(p)...);
		}
		static void Trim()noxnd
		{
			instance()._Trim();
		}
	private:
		template<class T, typename ...Params>
		std::shared_ptr<T> _Resolve(Graphics& gfx, Params&& ...p)noxnd
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);

			if (auto it = binds.find(key); it != binds.end())
				return std::static_pointer_cast<T>(it->second);

			binds.insert({ key, std::make_shared<T>(gfx, std::forward<Params>(p)...) });
			return std::static_pointer_cast<T>(binds.at(key));
		}
		template<class T, typename ...Params>
		concurrency::task<std::shared_ptr<T>> _ResolveAsync(Graphics& gfx, Params ...p)noxnd
		{
			co_await winrt::resume_background();
			const auto key = T::GenerateUID(std::forward<Params>(p)...);

			if (auto it = binds.find(key); it != binds.end())
				co_return std::static_pointer_cast<T>(it->second);

			if (auto it = tasks.find(key); it != tasks.end())
				co_return std::static_pointer_cast<T>(co_await it->second);//enforced

			{
				se.acquire();
				if (auto it = tasks.find(key); it != tasks.end())
				{
					se.release();
					co_return std::static_pointer_cast<T>(co_await it->second);//enforced
				}
				tasks.emplace(key, MakeAsync<T>(gfx, std::forward<Params>(p)...));
				se.release();
			}
			binds.insert({ key, co_await tasks.at(key) });
			co_return std::static_pointer_cast<T>(binds.at(key));
		}

		template<class T, typename ...Params>
		concurrency::task<std::shared_ptr<Bindable>> MakeAsync(Graphics& gfx, Params&& ...p)
		{
			co_return co_await ver::make_shared_async<T>(gfx, std::forward<Params>(p)...);
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
				binds.unsafe_erase(*bind);
		}
		static Codex& instance()
		{
			static Codex codex;
			return codex;
		}
	private:
		concurrency::concurrent_unordered_map<std::string, std::shared_ptr<Bindable>> binds;
		std::unordered_map<std::string, concurrency::task<std::shared_ptr<Bindable>>> tasks;
		std::binary_semaphore se{ 1 };
	};
}
