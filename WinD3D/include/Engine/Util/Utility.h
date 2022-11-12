#pragma once
#include <string>
#include <iterator>

#pragma warning(disable:6255) //alloca annoying warning

inline std::wstring ToWide(std::string_view narrowPtr)
{
	wchar_t* wide = (wchar_t*)alloca((narrowPtr.length() + 1) * sizeof(wchar_t));
	mbstowcs_s(nullptr, wide, narrowPtr.length() + 1, narrowPtr.data(), _TRUNCATE);
	return wide;
}
inline std::wstring ToWide(const std::string& narrow)
{
	return ToWide(std::string_view(narrow));
}

inline std::string ToNarrow(const std::wstring& wide)
{
	char* narrow = (char*)alloca((wide.length() + 1) * sizeof(char));
	wcstombs_s(nullptr, narrow, wide.length() + 1, wide.c_str(), _TRUNCATE);
	return narrow;
}

template<class Iter>
void SplitStringIter(const std::string& s, std::string_view delim, Iter out)
{
	if (delim.empty())
	{
		*out++ = s;
	}
	else
	{
		size_t a = 0, b = s.find(delim);
		for (; b != std::string::npos;
			a = b + delim.length(), b = s.find(delim, a))
		{
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

inline std::vector<std::string> SplitString(const std::string& s, std::string_view delim)
{
	std::vector<std::string> strings;
	SplitStringIter(s, delim, std::back_inserter(strings));
	return strings;
}