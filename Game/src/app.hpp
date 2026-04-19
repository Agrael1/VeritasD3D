#pragma once
#include "settings.hpp"
#include "window.hpp"

namespace out {
class App
{
public:
    App(const out::Config& config);

public:
    int Start();

private:
    out::Window _window;
};
} // namespace out
