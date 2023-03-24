#pragma once
#include <Shared/Timer.h>
#include <Shared/Definitions.h>
#include <Shared/Log.h>

namespace ver
{
	template<bool x>
	class scoped_profiler_p;

	template<>
	class scoped_profiler_p<true>
	{
	public:
		scoped_profiler_p(std::string message = "")
			:message(std::move(message)) {
			t.start();
		}
		~scoped_profiler_p()
		{
			ver::spd_info(std::format("{}: {}", message, t.stop()));
		}
	private:
		ver::timer t;
		std::string message;
	};

	template<>
	class scoped_profiler_p<false> { scoped_profiler_p(auto x) {} };

	using scoped_profiler = scoped_profiler_p<ver::debug_mode>;
}