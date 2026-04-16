#include "window.hpp"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

out::WindowLibrary::WindowLibrary() { SDL_Init(SDL_INIT_VIDEO); }

out::WindowLibrary::~WindowLibrary() { SDL_Quit(); }

out::Window::Window(const Config& settings)
    : window(SDL_CreateWindow(
          "Open UT",
          settings.window.width,
          settings.window.height,
          settings.fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE
      ))
{
    spdlog::info(
        "Created window with resolution {}x{} (fullscreen: {})",
        settings.window.width,
        settings.window.height,
        settings.fullscreen
    );
}

out::Window::~Window()
{
    if (window) {
        SDL_DestroyWindow(window);
    }
}
