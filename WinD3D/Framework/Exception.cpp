#include <Framework\Exception.h>
#include <sstream>

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
const std::string & Exception::GetFile() const noexcept
{
	return file;
}
std::string Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
const char* Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
