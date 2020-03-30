#pragma once
#include "BindingPass.h"

namespace RG
{
	class FullscreenPass : public BindingPass
	{
	public:
		FullscreenPass(const std::string name, Graphics& gfx) noxnd;
		void Execute(Graphics& gfx) const noxnd override;
	};
}