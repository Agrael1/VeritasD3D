#include <Engine/Graph/GalaxyRenderGraph.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/BufferClearPass.h>
#include <memory>
#include <Engine/Pass/FilteringPass.h>

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
        auto pass = std::make_unique<LambertianPass2>(gfx, "lambertian");
        pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
        pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
        AppendPass(std::move(pass));
    }
    SetSinkTarget("backbuffer", "lambertian.renderTarget");
    Finalize();
}