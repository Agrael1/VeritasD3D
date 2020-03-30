#pragma once
#include "Exception.h"

#define RGC_EXCEPTION( message ) RenderGraphCompileException( (message),__LINE__,__FILE__ )

namespace RG
{
	class RenderGraphCompileException : public Exception
	{
	public:
		RenderGraphCompileException(std::string message, int line, const char* file) noexcept;
	public:
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetMessage() const noexcept;
	private:
		std::string message;
	};
}