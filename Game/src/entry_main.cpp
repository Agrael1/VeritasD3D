#include "app.hpp"
#include <iostream>

int main(int argc, char** argv)
try {
    out::Config config = ver::cli::ParseCommandLineArgs<out::Config>(argc, argv);
    out::WindowLibrary window_library; // ensure SDL is initialized before any windows are created
    out::App app(config);
    return 0;
} catch (...) {
    std::cerr << "An unknown exception occurred.\n";
}
