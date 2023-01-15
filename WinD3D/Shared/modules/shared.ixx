module;
#define MODULE
//#include <format>
#include <spdlog/spdlog.h>
#include <Shared/Defines.h>
#include <source_location>
#include <string>
//#include <winrt/base.h>
export module shared;
import winrt;

#include <Shared/Checks.h>
#include <Shared/Severity.h>
#include <Shared/DXGIInfoManager.h>
#include <Shared/Log.h>
#include <Shared/Exception.h>
#include <Shared/Timer.h>