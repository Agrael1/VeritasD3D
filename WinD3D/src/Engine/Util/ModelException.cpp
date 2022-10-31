#include <Engine/Util/ModelException.h>
#include <format>

using namespace ver;

ModelException::ModelException(std::string note, std::source_location sl) noexcept
	:exception(sl),
	_note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	whatBuffer = std::format("{}\n[Note]: {}", ver::exception::what(), note());
	return whatBuffer.c_str();
}
