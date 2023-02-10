#pragma once
#include <Engine/Util/Exception.h>

namespace ver
{
	class ModelException : public exception
	{
	public:
		ModelException(std::string note, std::source_location sl = std::source_location::current()) noexcept;
	public:
		const char* what() const noexcept override;
		std::string_view type() const noexcept override
		{
			return "Veritas Model Exception";
		}
		std::string_view note() const noexcept
		{
			return _note;
		}
	private:
		std::string _note;
	};
}
