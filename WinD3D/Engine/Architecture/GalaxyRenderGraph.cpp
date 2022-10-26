#include "GalaxyRenderGraph.h"
#include "LambertianPass.h"
#include "BufferClearPass.h"
#include <memory>

RG::GalaxyRenderGraph::GalaxyRenderGraph(Graphics& gfx)
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
	SetSinkTarget("backbuffer", "lambertian.renderTarget");
	Finalize();
}