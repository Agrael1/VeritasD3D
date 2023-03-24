#pragma once

namespace ver
{
	template<class Impl>
	class Internal
	{
		static_assert(requires{Internal<Impl>::valid; }, "Internal class may be used only by explicit types");
	};

	template<class Impl>
	class APIBase
	{
	protected:
		const Impl& impl()const { return static_cast<const Impl&>(*this); }
		Impl& impl(){ return static_cast<Impl&>(*this); }
	};
}