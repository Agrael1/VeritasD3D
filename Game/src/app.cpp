#include "app.hpp"

int out::App::Start() { return 0; }

//----------------------------------------------------------------------------------------------------------------------
out::App::App(int argc, char** argv)
    : _settings("settings.ini")
    , _window(_settings.Get())
{}
