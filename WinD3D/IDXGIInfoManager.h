#pragma once
#include <vector>
#include <string>

struct IDXGIInfoManager
{
	virtual ~IDXGIInfoManager() = default;
	virtual void Set()noexcept = 0;
	virtual std::vector<std::string> GetMessages()const = 0;
};