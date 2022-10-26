#include "DeferredRenderGraph.h"
#include "LambertianPass.h"
#include "BufferClearPass.h"
#include "GBufferPass.h"
#include "LightingPass.h"
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
		auto pass = std::make_unique<GBufferPass>(gfx, "gbuf");
		pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LightingPass>(gfx, "light");
		pass->SetSinkLinkage("targets", "gbuf.targets");
		pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
		pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
		AppendPass(std::move(pass));
	}
	SetSinkTarget("backbuffer", "light.renderTarget");
	Finalize();
}
