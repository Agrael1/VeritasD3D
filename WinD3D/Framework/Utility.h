#pragma once
#include <string>

#pragma warning(disable:6255) //alloca annoying warning

inline std::wstring ToWide(std::string_view narrowPtr)
{
	wchar_t* wide = (wchar_t*)alloca((narrowPtr.length() + 1) * sizeof(wchar_t));
	mbstowcs_s(nullptr, wide, narrowPtr.length() + 1, narrowPtr.data(), _TRUNCATE);
	return wide;
}
inline std::wstring ToWide(const std::string& narrow)
{
	wchar_t* wide = (wchar_t*)alloca((narrow.length() + 1) * sizeof(wchar_t));
	mbstowcs_s(nullptr, wide, narrow.length() + 1, narrow.c_str(), _TRUNCATE);
	return wide;
}

inline std::string ToNarrow(const std::wstring& wide)
{
	char* narrow = (char*)alloca((wide.length() + 1) * sizeof(char));
	wcstombs_s(nullptr, narrow, wide.length() + 1, wide.c_str(), _TRUNCATE);
	return narrow;
}