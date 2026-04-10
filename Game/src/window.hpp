#pragma once
#include "settings.hpp"

struct SDL_Window;

namespace out {
struct WindowLibrary {
    WindowLibrary();
    ~WindowLibrary();
};

class Window
{
public:
    Window(Config& settings);
    ~Window();

public:

private:
    SDL_Window* window;
};
} // namespace out
