#include "RenderGraphException.h"
#include <fmt\printf.h>

namespace RG
{
	RenderGraphCompileException::RenderGraphCompileException(std::string message, int line, const char* file) noexcept
		:
		Exception(line, file),
		message(std::move(message))
	{}

	const char* RenderGraphCompileException::what() const noexcept
	{
		whatBuffer = fmt::sprintf("%s\n[message]:\n%s", Exception::what(), message);
		return whatBuffer.c_str();
	}
	const char* RenderGraphCompileException::GetType() const noexcept
	{
		return "Render Graph Compile Exception";
	}
	const std::string& RenderGraphCompileException::GetMessage() const noexcept
	{
		return message;
	}
}
