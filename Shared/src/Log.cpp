#include <Shared/Log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

ver::Log::Log() {
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	core = spdlog::stdout_color_mt("Core");
}