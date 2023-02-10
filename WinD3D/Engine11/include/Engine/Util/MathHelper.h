#pragma once
#include <DirectXMath.h>

template <typename T>
constexpr auto sq(const T& x) noexcept
{
	return x * x;
}
template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)DirectX::XM_2PI * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}