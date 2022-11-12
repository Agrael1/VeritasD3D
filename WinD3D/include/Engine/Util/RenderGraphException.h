#pragma once
#include "Exception.h"

#define RGC_EXCEPTION( message ) ver::make_error<ver::rg::RenderGraphCompileException>({message});

namespace ver::rg
{
	class RenderGraphCompileException : public ver::exception
	{
	public:
		RenderGraphCompileException(std::string message, std::source_location sl = std::source_location::current()) noexcept;
	public:
		const char* what() const noexcept override;
		std::string_view type() const noexcept override
		{
			return "Render Graph Compile Exception";
		}
		std::string_view message() const noexcept
		{
			return _message;
		}
	private:
		std::string _message;
	};
}