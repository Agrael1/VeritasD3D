#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Pass/GBufferPass.h>
#include <Engine/Pass/LightingPass.h>
#include <Engine/Pass/SkyboxPass.h>
#include <Engine/Pass/ShadowPass.h>
#include <memory>

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
		auto pass = std::make_unique<LightingPass>(gfx, "light");
		pass->SetSinkLinkage("shadow", "shadow.map");
		pass->SetSinkLinkage("targets", "lambertian.targets");
		pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
		pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<ver::rg::SkyboxPass>(gfx, "sky");
		pass->SetSinkLinkage("renderTarget", "light.renderTarget");
		pass->SetSinkLinkage("depthStencil", "light.depthStencil");
		AppendPass(std::move(pass));
	}	
	{
		auto pass = std::make_unique<LambertianPass>(gfx, "forward");
		pass->SetSinkLinkage("renderTarget", "sky.renderTarget");
		pass->SetSinkLinkage("depthStencil", "sky.depthStencil");
		AppendPass(std::move(pass));
	}

	SetSinkTarget("backbuffer", "forward.renderTarget");
	Finalize();
}
