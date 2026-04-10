#pragma once
#include "Bindable.h"

namespace ver
{
	class FullscreenEffect : public Bindable
	{
	public:
		// Inherited via Bindable
		virtual void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	};
}