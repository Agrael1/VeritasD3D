#include <Engine/Util/ModelException.h>
#include <format>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	Exception(line, file),
	note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	whatBuffer = std::format("{}\n[Note]: {}", Exception::what(), GetNote());
	return whatBuffer.c_str();
}
const char* ModelException::GetType() const noexcept
{
	return "Veritas Model Exception";
}
const std::string& ModelException::GetNote() const noexcept
{
	return note;
}