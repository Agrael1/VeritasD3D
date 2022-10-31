#include <Engine/Util/RenderGraphException.h>
#include <format>

using namespace ver::rg;

RenderGraphCompileException::RenderGraphCompileException(std::string message, std::source_location sl) noexcept
	:
	ver::exception(sl),
	_message(std::move(message))
{}

const char* RenderGraphCompileException::what() const noexcept
{
	if(whatBuffer.empty())
		whatBuffer = std::format("{}\n[message]:\n{}", ver::exception::what(), message());
	return whatBuffer.c_str();
}
