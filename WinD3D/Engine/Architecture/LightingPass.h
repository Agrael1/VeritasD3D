#pragma once
#include "BindingPass.h"
#include "RenderTarget.h"

namespace RG
{
	class LightingPass : public BindingPass
	{
	public:
		LightingPass(Graphics& gfx, std::string name) noxnd;
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<RenderTargetArray> rts;
	};
}