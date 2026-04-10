#pragma once
#include "settings.hpp"
#include "window.hpp"

namespace out {
class App
{
public:
    App(int argc, char** argv);

public:
    int Start();

private:
    Settings _settings;
    Window _window;
};
} // namespace out
