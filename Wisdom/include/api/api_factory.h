#pragma once
#include <api/api_adapter.h>
#include <util/generator.h>


namespace ver
{
	/// @brief Enumeration specifying Adapter query ordering
	enum class AdapterPreference
	{
		None, //< Default order (defined by system)
		MinConsumption, //< Order:Integrated, Discrete
		Performance //< Order: Discrete, Integrated
	};


	template<class Impl>
	class APIFactory : public APIBase<Impl>
	{
		using APIBase<Impl>::impl;
	public:
		template<class AdapterType>
		ver::generator<APIAdapter<AdapterType>> EnumerateAdapters(AdapterPreference preference = AdapterPreference::Performance)const noexcept
		{
			return impl().EnumerateAdapters(preference);
		}
	};
}