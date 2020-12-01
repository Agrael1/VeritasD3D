#pragma once
#include "FullscreenPass.h"
#include "ConstantBuffersEx.h"

namespace RG
{
	class HorizontalBlurPass : public FullscreenPass
	{
	public:
		HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight);
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<CachingPixelConstantBufferEx> direction;
	};
}