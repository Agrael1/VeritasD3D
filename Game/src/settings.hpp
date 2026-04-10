//
// Created by Agrael on 4/10/2026.
//

#pragma once
#include <veritas/base/settings.hpp>

namespace out {
struct WindowSettings {
    int32_t width = 1280;
    int32_t height = 720;

    constexpr bool operator==(const WindowSettings& other) const
    {
        return width == other.width && height == other.height;
    }
};
struct Config {
    WindowSettings window;
    bool fullscreen = false;
};

using Settings = ver::Settings<Config, "resolution", "fullscreen">;
} // namespace out

// Specify format and parse functions for each setting type here, so that the Settings class can use them to read/write
// settings from/to the file
namespace std {

template <>
struct formatter<out::WindowSettings> : formatter<string_view> {
    auto format(const out::WindowSettings& settings, format_context& ctx) const
    {
        return formatter<string_view>::format(std::format("{}x{}", settings.width, settings.height), ctx);
    }
};
} // namespace std

namespace ver {
template <>
struct ParseValue<out::WindowSettings> {
    ParseValue(out::WindowSettings& target, std::string_view val)
    {
        auto index = val.find('x');
        if (index == std::string_view::npos) {
            spdlog::warn("Invalid window settings format: '{}'", val);
            return;
        }

        ParseValue<int32_t>(target.width, val.substr(0, index));
        ParseValue<int32_t>(target.height, val.substr(index + 1));
    }
};
} // namespace ver
