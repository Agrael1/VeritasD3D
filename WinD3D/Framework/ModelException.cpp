#include "ModelException.h"
#include <fmt/printf.h>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	Exception(line, file),
	note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	whatBuffer = fmt::sprintf("%s\n[Note]: %s", Exception::what(), GetNote());
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