#pragma once

template<class T, typename C>
decltype(auto) convert(C&& from)
{
	static_assert(sizeof(T) == sizeof(C));
	return reinterpret_cast<T&&>(from);
}