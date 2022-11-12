#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Pass/GBufferPass.h>
#include <Engine/Pass/LightingPass.h>
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
		auto pass = std::make_unique<GBufferPass>(gfx, "lambertian");
		pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LightingPass>(gfx, "light");
		pass->SetSinkLinkage("targets", "lambertian.targets");
		pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
		pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
		AppendPass(std::move(pass));
	}
	//{
	//	auto pass = std::make_unique<TransparentPass>(gfx, "transparent");
	//	pass->SetSinkLinkage("renderTarget", "light.renderTarget");
	//	pass->SetSinkLinkage("depthStencil", "light.depthStencil");
	//	AppendPass(std::move(pass));
	//}
	SetSinkTarget("backbuffer", "light.renderTarget");
	Finalize();
}
