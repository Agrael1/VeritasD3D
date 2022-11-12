#pragma once
#include <exception>
#include <string>
#include <source_location>

namespace ver
{
	class exception :std::exception
	{
	public:
		exception(std::source_location sl = std::source_location::current())noexcept;
	public:
		void WriteToOutput()const noexcept;
		const char* what()const noexcept override;
		virtual std::string_view type()const noexcept
		{
			return "Veritas exception";
		}
		uint32_t line()const noexcept
		{
			return sl.line();
		}
		std::string_view file()const noexcept
		{
			return sl.file_name();
		}
		std::string_view function()const noexcept
		{
			return sl.function_name();
		}
		std::string origin()const noexcept;
	private:
		std::source_location sl;
	protected:
		mutable std::string whatBuffer;
	};


	template<class E, class ... Args> requires ((bool)DEBUG_MODE)
	E make_error(E in)
	{
		in.WriteToOutput();
		return in;
	}
	template<class E, class ... Args>requires (!(bool)DEBUG_MODE)
	E make_error(Args&&...args, std::source_location sl = std::source_location::current())
	{
		return { args..., sl };
	}

}