#include "app.hpp"

int out::App::Start() { return 0; }

//----------------------------------------------------------------------------------------------------------------------
out::App::App(const out::Config& config) 
: _window(config.window)
{
}
