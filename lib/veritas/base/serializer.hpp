#pragma once
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <veritas/base/parse.hpp>

namespace ver {
template <typename T>
struct Schema;

template <typename Class, typename Type>
constexpr Class ExtractClass(Type Class::*);

template <auto MemberPtr, std::size_t NameCount>
struct FieldBinding {
    using ClassType = decltype(ExtractClass(MemberPtr));
    std::array<std::string_view, NameCount> names;

    // A static stub we will store in the frozen::unordered_map
    template <template <typename> typename ParseType>
    static constexpr void ParseStub(ClassType& obj, std::string_view value)
    {
        ParseType(obj.*MemberPtr, value);
    }
};

template <auto MemberPtr, typename... StringViews>
constexpr auto BindField(StringViews... names)
{
    return FieldBinding<MemberPtr, sizeof...(names)>{{names...}};
}

template <typename TStruct>
struct Serializer {
    using ParseFunc = void (*)(TStruct&, std::string_view);

    // Build the O(1) jump table at compile time
    static constexpr auto call_table = []() {
        constexpr auto tuple = Schema<TStruct>::define();

        // 1. Calculate the total number of aliases to size our map
        constexpr std::size_t total_names = std::apply(
            [](auto... bindings) { return (bindings.names.size() + ...); },
            tuple
        );

        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            // Helper to generate a valid empty pair
            auto make_dummy = [](auto) { return std::pair<frozen::string, ParseFunc>{frozen::string(""), nullptr}; };

            // Unpack the dummy elements into the array
            std::array<std::pair<frozen::string, ParseFunc>, total_names> entries{make_dummy(Is)...};

            std::size_t index = 0;
            auto fill_entries = [&entries, &index](auto binding) {
                for (auto name : binding.names) {
                    entries[index++] = {frozen::string(name.data(), name.size()), binding.ParseStub<ParseValue>};
                }
            };

            std::apply([&](auto... bindings) { (fill_entries(bindings), ...); }, tuple);

            return frozen::unordered_map<frozen::string, ParseFunc, total_names>{entries};
        }(std::make_index_sequence<total_names>{});
    }();
};

// Concept to check if a type has a defined Schema
template <typename T>
concept HasSchema = requires { Schema<T>::define(); };
} // namespace ver
