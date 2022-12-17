#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Pass/GBufferPass.h>
#include <Engine/Pass/LightingPass.h>
#include <Engine/Pass/ShadowPass.h>
#include <Engine/Pass/AmbientPass.h>
#include <memory>
#include <Engine/Pass/FilteringPass.h>

RG::DeferredRenderGraph::DeferredRenderGraph(Graphics& gfx)
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
		auto pass = std::make_unique<ver::rg::ShadowMappingPass>(gfx, "shadow");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<GBufferPass>(gfx, "lambertian");
		pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<ver::rg::AmbientPass>(gfx, "ambient");
		pass->SetSinkLinkage("targets", "lambertian.targets");
		pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LightingPass>(gfx, "light");
		pass->SetSinkLinkage("shadow", "shadow.map");
		pass->SetSinkLinkage("targets", "ambient.targets");
		pass->SetSinkLinkage("depthStencil", "ambient.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LambertianPass>(gfx, "forward");
		pass->SetSinkLinkage("renderTarget", "light.renderTarget");
		pass->SetSinkLinkage("depthStencil", "light.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<ver::rg::FilteringPass>(gfx, "filter");
		pass->SetSinkLinkage("preFilterTarget", "forward.renderTarget");
		pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
		AppendPass(std::move(pass));
	}

	SetSinkTarget("backbuffer", "filter.renderTarget");
	Finalize();
}
