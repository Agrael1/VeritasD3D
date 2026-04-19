#pragma once
#include <spdlog/spdlog.h>
#include <charconv>
#include <concepts>

namespace ver {

template <typename TValue>
struct ParseValue {
    ParseValue(TValue& target, std::string_view val) {}
};

template <>
struct ParseValue<bool> {
    ParseValue(bool& target, std::string_view val) { target = (val != "false" && val != "0"); }
};

template <std::integral TIntegral>
struct ParseValue<TIntegral> {
    ParseValue(TIntegral& target, std::string_view val)
    {
        // use std::from_chars for integral types
        auto result = std::from_chars(val.data(), val.data() + val.size(), target);
        if (result.ec != std::errc()) {
            spdlog::warn("Failed to parse integral value: '{}'", val);
        }
    }
};

template <typename TString>
concept StringLike = std::is_convertible_v<TString, std::string_view>;

// String parsing is trivial, just assign the value
template <StringLike TString>
struct ParseValue<TString> {
    ParseValue(TString& target, std::string_view val) { target = TString{val}; }
};

} // namespace ver
