#pragma once
#include <Engine/Rendering/RenderGraph.h>
#include <memory>
#include <Engine/Bindable/ConstantBuffersEX.h>

class Graphics;
class Bindable;
class RenderTarget;


namespace RG
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
	private:
		// private functions
		void SetKernelGauss(int radius, float sigma) noxnd;
		// private data
		static constexpr int maxRadius = 7;
		static constexpr int radius = 4;
		static constexpr float sigma = 2.0f;
		std::shared_ptr<CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<CachingPixelConstantBufferEx> blurDirection;
	};
}