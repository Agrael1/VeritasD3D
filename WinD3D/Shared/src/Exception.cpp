#include <Shared/Exception.h>
#include <Shared/Log.h>
#include <format>

using namespace ver;

exception::exception(std::source_location sl) noexcept
	:sl(std::move(sl))
{}

std::string exception::origin() const noexcept
{
	return std::format("[File]: {}\n[Line]: {}\n[Function]: {}", file(), line(), function());
}
const char* exception::what() const noexcept
{
	if (whatBuffer.empty())
		whatBuffer = std::format("{}\n{}", type(), origin());
	return whatBuffer.c_str();
}

void ver::exception::WriteToOutput()const noexcept
{
	ver::std_critical(what());
	//OutputDebugStringA(what());
}