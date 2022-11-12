#include "ttfcpp.h"
#include <fstream>

ttf::ttf_bridge::ttf_bridge(std::filesystem::path p)
{
    std::ifstream file{ p, std::ios::binary | std::ios::in };
    std::string str;

    file.seekg(0, std::ios::end);
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    ttf_load_from_mem(reinterpret_cast<const uint8_t*>(str.c_str()), int(str.length()), &xttf, false);
}

