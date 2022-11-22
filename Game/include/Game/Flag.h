#pragma once
#include <Engine/Loading/Model.h>

namespace UT
{
	class Flag
	{
	public:
		Flag() = default;
	public:
		winrt::IAsyncAction InitializeAsync(Graphics& gfx);
		auto* operator->()
		{
			return &model;
		}
	private:
		Model model;
	};
}