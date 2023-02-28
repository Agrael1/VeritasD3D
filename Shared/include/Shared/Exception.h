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

	class hr_exception :public ver::exception
	{
	public:
		hr_exception(winrt::hresult hr, std::source_location sl = std::source_location::current());
	public:
		const char* what() const noexcept override;
		std::string_view type()const noexcept override
		{
			return "Vertas Window Exception";
		}
		winrt::hresult error_code() const noexcept
		{
			return hResult;
		}
		std::string description() const noexcept;
	private:
		winrt::hresult hResult;
	};
}