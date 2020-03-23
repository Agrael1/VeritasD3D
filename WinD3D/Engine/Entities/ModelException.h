#pragma once
#include <Framework/Exception.h>

class ModelException : public Exception
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
public:
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};