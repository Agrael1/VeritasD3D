#pragma once
#include <veritas/base/serializer.hpp>

namespace ver::cli {
template <typename TStruct>
void ParseRecurse(TStruct& config, std::string_view arg)
{
    using CLISerializer = Serializer<TStruct>;

    auto split_pos = arg.find_first_of(".=");
    std::string_view name = arg.substr(0, split_pos);
    std::string_view value;

    if (split_pos != std::string_view::npos) {
        value = arg.substr(split_pos + 1);
    }

    auto it = CLISerializer::call_table.find(name);
    if (it != CLISerializer::call_table.end()) {
        it->second(config, value); // Call the parse function
    } else {
        spdlog::warn("Unknown argument '{}'", name);
    }
}

template <typename TStruct>
TStruct ParseCommandLineArgs(int argc, char* argv[])
{
    TStruct config{}; // Default-initialized config for comparison
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg.starts_with("--")) {
            arg.remove_prefix(2);
        } else if (arg.starts_with("-")) {
            arg.remove_prefix(1);
        } else {
            spdlog::warn("Ignoring invalid argument '{}'", arg);
            continue;
        }

        ParseRecurse(config, arg);
    }
    return config;
}
} // namespace ver::cli
