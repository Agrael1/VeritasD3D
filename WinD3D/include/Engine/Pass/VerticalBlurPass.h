#pragma once
#include "FullscreenPass.h"
#include <Engine/Bindable/ConstantBuffersEX.h>

namespace RG
{
	class VerticalBlurPass : public FullscreenPass
	{
	public:
		VerticalBlurPass(std::string name, Graphics& gfx);
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<CachingPixelConstantBufferEx> direction;
	};
}