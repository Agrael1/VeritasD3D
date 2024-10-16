#include <Engine/Graph/BlurOutlineRenderGraph.h>
#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/OutlineMaskGenerationPass.h>
#include <Engine/Pass/BlurOutlineDrawingPass.h>
#include <Engine/Pass/HorizontalBlurPass.h>
#include <Engine/Pass/VerticalBlurPass.h>
#include <Engine/Util/MathHelper.h>
#include <Engine/Graphics.h>

namespace RG
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
		:RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}

		// setup blur constant buffers
		{
			{
				DC::RawLayout l;
				l.Add({
					{DC::Type::Integer, "nTaps"},
					{DC::Type::Float, "coefficients", maxRadius * 2 + 1}
				});
				DC::Buffer buf{ std::move(l) };
				blurKernel = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectBindableSource<CachingPixelConstantBufferEx>::Make("blurKernel", blurKernel));
			}
			{
				DC::RawLayout l;
				l.Add({ {DC::Type::Bool,"isHorizontal"} });
				DC::Buffer buf{ std::move(l) };
				blurDirection = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectBindableSource<CachingPixelConstantBufferEx>::Make("blurDirection", blurDirection));
			}
		}

		{
			auto pass = std::make_unique<BlurOutlineDrawingPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "vertical.renderTarget");

		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noxnd
	{
		assert(radius <= maxRadius);
		auto k = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;
		for (int i = 0; i < nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k["coefficients"][i] = g;
		}
		for (int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}
		blurKernel->SetBuffer(k);
	}
}