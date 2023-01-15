#pragma once
#include <spdlog/spdlog.h>
#include <Shared/Severity.h>

namespace ver
{
	class Log
	{
	public:
		static Log& Instance() { static Log l; return l; }
	public:
		spdlog::logger& GetCoreLog() { return *core; }
	private:
		Log();
	private:
		std::shared_ptr<spdlog::logger> core;
	};

	template<class ...Args>
	inline void std_warn(Args&&... args) { Log::Instance().GetCoreLog().warn(std::forward<Args>(args)...); }
	template<class ...Args>
	inline void std_info(Args&&... args) { Log::Instance().GetCoreLog().info(std::forward<Args>(args)...); }
	template<class ...Args>
	inline void std_error(Args&&... args) { Log::Instance().GetCoreLog().error(std::forward<Args>(args)...); }
	template<class ...Args>
	inline void std_critical(Args&&... args) { Log::Instance().GetCoreLog().critical(std::forward<Args>(args)...); }
	template<class ...Args>
	inline void std_debug(Args&&... args) { Log::Instance().GetCoreLog().debug(std::forward<Args>(args)...); }
	template<class ...Args>
	inline void std_trace(Args&&... args) { Log::Instance().GetCoreLog().trace(std::forward<Args>(args)...); }

	template<class ...Args>
	inline void std_log(Severity sev, Args&&... args)
	{
		using enum Severity;
		switch (sev)
		{
		case ver::Severity::debug:
			return std_debug(std::forward<Args>(args)...);
		case ver::Severity::trace:
			return std_trace(std::forward<Args>(args)...);
		case ver::Severity::info:
			return std_info(std::forward<Args>(args)...);
		case ver::Severity::warn:
			return std_warn(std::forward<Args>(args)...);
		case ver::Severity::error:
			return std_error(std::forward<Args>(args)...);

		default:
		case ver::Severity::critical:
			return std_critical(std::forward<Args>(args)...);
		}
	}
}