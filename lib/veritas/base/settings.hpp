#pragma once
#include <wisdom/util/string_literal.hpp>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <veritas/base/parse.hpp>

#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>

// Persistent settings for the engine. These settings are stored in a file and loaded on startup. They can be modified
// by the user and saved back to the file.
namespace ver {
template <typename TSetting, wis::fixed_string... SettingNames>
class Settings
{
    static constexpr std::size_t member_count = sizeof...(SettingNames);

    using SettingMap = frozen::unordered_map<frozen::string, std::size_t, member_count>;
    using AssignFunc = void (*)(TSetting&, std::string_view);

    static constexpr std::string_view setting_names[]{std::string_view(SettingNames)...};
    static constexpr std::array<AssignFunc, member_count> jump_table = []<std::size_t... I>(std::index_sequence<I...>) {
        return std::array<AssignFunc, member_count>{
            // The '+' forces the captureless lambda to decay into a function pointer
            +[](TSetting& target, std::string_view val) {
                auto& [... parts] = target;

                // C++26 PACK INDEXING + constructor abuse!
                ParseValue(parts...[I], val);
            }...
        };
    }(std::make_index_sequence<member_count>{});

    static constexpr SettingMap setting_name_to_index = []<std::size_t... I>(std::index_sequence<I...>) {
        return SettingMap{{frozen::string(setting_names[I]), std::size_t(I)}...};
    }(std::make_index_sequence<member_count>{});

public:
    Settings(std::filesystem::path&& path);
    ~Settings();

public:
    const TSetting& Get() const noexcept { return _current_settings; }
    TSetting& Get() noexcept { return _current_settings; }

    void Save() const;
    void Load();

private:
    std::filesystem::path _file_path;
    TSetting _current_settings;
};

template <typename TSetting, wis::fixed_string... SettingNames>
inline Settings<TSetting, SettingNames...>::Settings(std::filesystem::path&& path)
    : _file_path(std::move(path))
{
    static_assert(member_count > 0, "At least one setting name must be provided.");
    static constexpr auto [... parts] = TSetting{};
    static_assert(
        sizeof...(parts) == member_count,
        "The number of setting names must match the number of settings in TSetting."
    );
    Load();
}
template <typename TSetting, wis::fixed_string... SettingNames>
Settings<TSetting, SettingNames...>::~Settings()
{
    Save();
}

template <typename TSetting, wis::fixed_string... SettingNames>
void Settings<TSetting, SettingNames...>::Save() const
{
    constexpr static auto [... default_params] = TSetting{};

    const auto& [... params] = _current_settings;
    std::string output;

    static auto write_if_changed = [&](const auto& param, const auto& default_param, std::string_view name) {
        if (!(param == default_param)) {
            std::format_to(std::back_inserter(output), "{}={}\n", name, param);
        }
    };

    // A fold expression to iterate over the settings and write only those that have changed from their default
    // values.
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        (..., (write_if_changed(params...[I], default_params...[I], setting_names[I])));
    }(std::make_index_sequence<sizeof...(params)>{});

    if (output.empty()) {
        return;
    }

    spdlog::info("Saving settings to {}:\n{}", _file_path.string(), output);
    std::ofstream file(_file_path);
    if (!file.is_open()) {
        spdlog::error("Failed to open settings file for writing: {}", _file_path.string());
        return;
    }
    file << output;
}

template <typename TSetting, wis::fixed_string... SettingNames>
void Settings<TSetting, SettingNames...>::Load()
{
    if (_file_path.empty()) {
        spdlog::info("Settings file path is empty, skipping load.");
        return;
    }

    if (!std::filesystem::exists(_file_path)) {
        spdlog::warn("Settings file not found at '{}', using default settings.", _file_path.string());
        return;
    }

    // Open the settings file, read each line, and use the jump table to assign values to the settings struct based on
    // the setting names.
    std::ifstream file(_file_path);
    if (!file.is_open()) {
        spdlog::error("Failed to open settings file for reading: {}", _file_path.string());
        return;
    }

    // read the whole file into a string and split it into lines
    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    for (const auto line : std::views::split(std::string_view(file_content), '\n')) {
        auto line_view = std::string_view(line);

        auto delimiter_pos = line_view.find('=');
        if (delimiter_pos == std::string_view::npos) {
            spdlog::warn("Invalid settings line (missing '='): '{}'", line_view);
            continue;
        }
        auto name = line_view.substr(0, delimiter_pos);
        auto value = line_view.substr(delimiter_pos + 1);

        auto it = setting_name_to_index.find(name);
        if (it == setting_name_to_index.end()) {
            spdlog::warn("Unknown setting name: '{}'", name);
            continue;
        }
        std::size_t index = it->second;
        jump_table[index](_current_settings, value);
    }
}
} // namespace ver
