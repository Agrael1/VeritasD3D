#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Pass/GBufferPass.h>
#include <Engine/Pass/LightingPass.h>
#include <Engine/Pass/ShadowPass.h>
#include <Engine/Pass/AmbientPass.h>
#include <memory>
#include <Engine/Pass/FilteringPass.h>
namespace ver
{
	class XFilteringPass : public RG::BindingPass, public ver::GraphicsResource
	{
	public:
		XFilteringPass(Graphics& gfx, std::string name) noxnd
			: BindingPass(std::move(name))
		{
			AddBindSink<IRenderTarget>("preFilterTarget");
			AddBindSink<IRenderTarget>("preFilterTarget2");
			RegisterSink(RG::DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(RG::DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));

			AddBind(Sampler::Resolve(gfx));

			AddBind(ver::VertexShader::Resolve(gfx, "fullscreen.vs.cso"));
			AddBind(ver::PixelShader::Resolve(gfx, "add_col.ps.cso"));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			renderTarget->BindAsBuffer(gfx);
			Topology::Bind(gfx);
			for (auto& bind : binds)
			{
				bind->Bind(gfx);
			}
			gfx.Draw(3u);
		}
	};
}



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
	//{
	//	auto pass = std::make_unique<ver::rg::FilteringPass>(gfx, "filter");
	//	pass->SetSinkLinkage("preFilterTarget", "forward.renderTarget");
	//	pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
	//	AppendPass(std::move(pass));
	//}

	//////////////////////////////////////////////////////////////////////2

	//gfx.SetCamera(gfx.GetCamera() * DirectX::XMMatrixTranslation(6.5, 0, 0));
	{
		auto pass = std::make_unique<BufferClearPass>("clearDS2");
		pass->SetSinkLinkage("buffer", "forward.depthStencil");
		AppendPass(std::move(pass));
	}
	//{
	//	auto pass = std::make_unique<ver::rg::ShadowMappingPass>(gfx, "shadow");
	//	AppendPass(std::move(pass));
	//}
	{
		auto pass = std::make_unique<GBufferPass>(gfx, "lambertian2");
		pass->SetSinkLinkage("depthStencil", "clearDS2.buffer");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<ver::rg::AmbientPass>(gfx, "ambient2");
		pass->SetSinkLinkage("targets", "lambertian2.targets");
		pass->SetSinkLinkage("depthStencil", "lambertian2.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LightingPass>(gfx, "light2",1);
		pass->SetSinkLinkage("shadow", "shadow.map");
		pass->SetSinkLinkage("targets", "ambient2.targets");
		pass->SetSinkLinkage("depthStencil", "ambient2.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LambertianPass>(gfx, "forward2");
		pass->SetSinkLinkage("renderTarget", "light2.renderTarget");
		pass->SetSinkLinkage("depthStencil", "light2.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<ver::XFilteringPass>(gfx, "filter2");
		pass->SetSinkLinkage("preFilterTarget", "forward.renderTarget");
		pass->SetSinkLinkage("preFilterTarget2", "forward2.renderTarget");
		pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
		AppendPass(std::move(pass));
	}



	SetSinkTarget("backbuffer", "filter2.renderTarget");
	Finalize();
}
