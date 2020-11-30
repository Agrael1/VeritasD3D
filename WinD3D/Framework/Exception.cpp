#include <Framework\Exception.h>
#include <fmt/printf.h>

Exception::Exception(unsigned int line, const char * file) noexcept
	:line(line),file(file)
{}

const char * Exception::GetType() const noexcept
{
	return "Veritas Exception";
}
int Exception::GetLine() const noexcept
{
	return line;
}
std::string_view Exception::GetFile() const noexcept
{
	return file;
}
std::string Exception::GetOriginString() const noexcept
{
	return fmt::sprintf("[File]: %s\n[Line]: %d", file, line);
}
const char* Exception::what() const noexcept
{
	if (whatBuffer.empty()) 
		whatBuffer = GetType() + '\n' + GetOriginString();
	return whatBuffer.c_str();
}
