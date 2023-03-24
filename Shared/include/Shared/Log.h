#pragma once
#include <spdlog/spdlog.h>
#include <Shared/Severity.h>

namespace ver
{
    inline void spd_trace(std::string message, std::source_location sl = std::source_location::current())
    {
        if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE)
            spdlog::log(spdlog::source_loc{ sl.file_name(), int(int(sl.line())), sl.function_name() }, spdlog::level::level_enum::trace, std::move(message));
    }
    inline void spd_debug(std::string message, std::source_location sl = std::source_location::current())
    {
        if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG)
            spdlog::log(spdlog::source_loc{ sl.file_name(), int(sl.line()), sl.function_name() }, spdlog::level::level_enum::debug, std::move(message));
    }
    inline void spd_info(std::string message, std::source_location sl = std::source_location::current())
    {
        if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO)
            spdlog::log(spdlog::source_loc{ sl.file_name(), int(sl.line()), sl.function_name() }, spdlog::level::level_enum::info, std::move(message));
    }
    inline void spd_warn(std::string message, std::source_location sl = std::source_location::current())
    {
        if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN)
            spdlog::log(spdlog::source_loc{ sl.file_name(), int(sl.line()), sl.function_name() }, spdlog::level::level_enum::warn, std::move(message));
    }
    inline void spd_error(std::string message, std::source_location sl = std::source_location::current())
    {
        if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR)
            spdlog::log(spdlog::source_loc{ sl.file_name(), int(sl.line()), sl.function_name() }, spdlog::level::level_enum::err, std::move(message));
    }
    inline void spd_critical(std::string message, std::source_location sl = std::source_location::current())
    {
        if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL)
            spdlog::log(spdlog::source_loc{ sl.file_name(), int(sl.line()), sl.function_name() }, spdlog::level::level_enum::critical, std::move(message));
    }

    inline void spd_log(Severity sev, std::string message, std::source_location sl = std::source_location::current())
    {
        switch (sev) {
        case Severity::debug:
            return spd_debug(message, sl);
        case Severity::trace:
            return spd_trace(message, sl);
        case Severity::info:
            return spd_info(message, sl);
        case Severity::warn:
            return spd_warn(message, sl);
        case Severity::error:
            return spd_error(message, sl);
        default:
        case Severity::critical:
            return spd_critical(message, sl);
        }
    }
}