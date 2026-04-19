//
// Created by Agrael on 4/10/2026.
//

#pragma once
#include <veritas/base/cli.hpp>
#include <format>

namespace out {
struct WindowSettings {
    int32_t width = 1280;
    int32_t height = 720;
    bool fullscreen = false;
};
struct Config {
    WindowSettings window;
};
} // namespace out

template <>
struct ver::Schema<out::Config> {
    static constexpr auto define()
    {
        return std::make_tuple(
            ver::BindField<&out::Config::window>("window", "w")
        );
    }
};

template <>
struct ver::Schema<out::WindowSettings> {
    static constexpr auto define()
    {
        return std::make_tuple(
            ver::BindField<&out::WindowSettings::width>("width", "w"),
            ver::BindField<&out::WindowSettings::height>("height", "h"),
            ver::BindField<&out::WindowSettings::fullscreen>("fullscreen", "f")
        );
    }
};

template <>
struct std::formatter<out::WindowSettings> : std::formatter<string_view> {
    auto format(const out::WindowSettings& settings, std::format_context& ctx) const
    {
        return formatter<string_view>::format(std::format("{}x{}", settings.width, settings.height), ctx);
    }
};

template <>
struct ver::ParseValue<out::WindowSettings> {
    ParseValue(out::WindowSettings& target, std::string_view val)
    {
        auto index = val.find('x');
        if (index != std::string_view::npos) {
            ParseValue<int32_t>(target.width, val.substr(0, index));
            ParseValue<int32_t>(target.height, val.substr(index + 1));
            return;
        }

        // Complex route for parsing nested fields like "window.width=1280"
        ver::cli::ParseRecurse(target, val);
    }
};
