#include <Engine/Util/RenderGraphException.h>
#include <format>

using namespace ver::rg;

RenderGraphCompileException::RenderGraphCompileException(std::string message, bool stacktrace, std::source_location sl) noexcept
	:
	ver::exception(sl),
	_message(std::move(message)), _trace(stacktrace?std::to_string(std::stacktrace::current()):"")
{}

const char* RenderGraphCompileException::what() const noexcept
{
	if (whatBuffer.empty())
	{
		whatBuffer = 
			_trace.empty() ? std::format("{}\n[message]:\n{}\n", ver::exception::what(), message()) :
			std::format("{}\n[message]:\n{}\n[trace]:\n", ver::exception::what(), message(), _trace);
	}
	return whatBuffer.c_str();
}
