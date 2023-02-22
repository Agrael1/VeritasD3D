#pragma once
#include <Engine/Rendering/RenderGraph.h>
#include <Engine/Pass/LambertianPass.h>
#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Pass/GBufferPass.h>
#include <Engine/Pass/LightingPass.h>
#include <Engine/Pass/ShadowPass.h>
#include <Engine/Pass/AmbientPass.h>
#include <memory>
#include <Engine/Pass/FilteringPass.h>
#include <Engine/Bindable/RenderTarget.h>

namespace ver
{
	class AnaglyphPass : public RG::BindingPass, public ver::GraphicsResource
	{
	public:
		AnaglyphPass(Graphics& gfx, std::string name) noxnd
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



	class CopyPass : public RG::BindingPass, public ver::GraphicsResource
	{
	public:
		CopyPass(Graphics& gfx, std::string name) noxnd
			: BindingPass(std::move(name))
		{
			AddBindSink<IRenderTarget>("preFilterTarget");
			RegisterSink(RG::DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(RG::DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));

			AddBind(Sampler::Resolve(gfx));
			AddBind(ver::VertexShader::Resolve(gfx, "fullscreen.vs.cso"));
			AddBind(ver::PixelShader::Resolve(gfx, "loading.ps.cso"));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			renderTarget->BindAsBuffer(gfx);
			Topology::Bind(gfx);
			for (auto& bind : binds)
			{
				//atrocities commited
				if (auto a = std::dynamic_pointer_cast<ShaderInputRenderTarget>(bind))
					a->BindTo(gfx, 0);
				else
					bind->Bind(gfx);
			}
			gfx.Draw(3u);
		}
	};
}

namespace ver::rg
{
	class StereoGraph : public RenderGraph
	{
	public:
		StereoGraph(Graphics& gfx)
			:RenderGraph(gfx)
			, auxDepth(std::make_shared<ver::ShaderInputDepthStencil>(gfx))
		{
			AddGlobalSource(DirectBufferSource<ver::DepthStencil>::Make("auxDepth", auxDepth));
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
				auto pass = std::make_unique<BufferClearPass>("clearDS2");
				pass->SetSinkLinkage("buffer", "$.auxDepth");
				AppendPass(std::move(pass));
			}


			{
				auto pass = std::make_unique<ver::rg::ShadowMappingPass>(gfx, "shadow");
				AppendPass(std::move(pass));
			}


			{
				auto pass = std::make_unique<StereoGBufferPass>(gfx, "lambertian");
				pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
				pass->SetSinkLinkage("auxDepth", "clearDS2.buffer");
				AppendPass(std::move(pass));
			}

			{
				auto pass = std::make_unique<ver::rg::AmbientPass>(gfx, "ambient");
				pass->SetSinkLinkage("targets", "lambertian.targets");
				pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
				AppendPass(std::move(pass));
			}
			{
				auto pass = std::make_unique<ver::rg::AmbientPass>(gfx, "ambient2");
				pass->SetSinkLinkage("targets", "lambertian.auxTargets");
				pass->SetSinkLinkage("depthStencil", "lambertian.auxDepth");
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
				auto pass = std::make_unique<LightingPass>(gfx, "light2", 1);
				pass->SetSinkLinkage("shadow", "shadow.map");
				pass->SetSinkLinkage("targets", "ambient2.targets");
				pass->SetSinkLinkage("depthStencil", "ambient2.depthStencil");
				AppendPass(std::move(pass));
			}

			{
				auto pass = std::make_unique<ver::rg::StereoLambertianPass>(gfx, "forward");
				pass->SetSinkLinkage("renderTarget", "light.renderTarget");
				pass->SetSinkLinkage("renderTarget2", "light2.renderTarget");
				pass->SetSinkLinkage("depthStencil", "light.depthStencil");
				pass->SetSinkLinkage("depthStencil2", "light2.depthStencil");
				AppendPass(std::move(pass));
			}

			if (!gfx.StereoEnabled())
			{
				auto pass = std::make_unique<ver::AnaglyphPass>(gfx, "filter");
				pass->SetSinkLinkage("preFilterTarget", "forward.renderTarget");
				pass->SetSinkLinkage("preFilterTarget2", "forward.renderTarget2");
				pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
				AppendPass(std::move(pass));

				SetSinkTarget("backbuffer", "filter.renderTarget");
			}
			else
			{
				rightBuffer = gfx.GetRightTarget();
				AddGlobalSource(DirectBufferSource<RenderTarget>::Make("rightbuffer", rightBuffer));

				{
					auto pass = std::make_unique<BufferClearPass>("clearRT2");
					pass->SetSinkLinkage("buffer", "$.rightbuffer");
					AppendPass(std::move(pass));
				}
				{
					auto pass = std::make_unique<ver::CopyPass>(gfx, "filter");
					pass->SetSinkLinkage("preFilterTarget", "forward.renderTarget");
					pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
					AppendPass(std::move(pass));
				}
				{
					auto pass = std::make_unique<ver::CopyPass>(gfx, "filter2");
					pass->SetSinkLinkage("preFilterTarget", "forward.renderTarget2");
					pass->SetSinkLinkage("renderTarget", "clearRT2.buffer");
					AppendPass(std::move(pass));
				}

				SetSinkTarget("backbuffer", "filter.renderTarget");
			}
			Finalize();
		}
	private:
		std::shared_ptr<ver::DepthStencil> auxDepth;
		std::shared_ptr<RenderTarget> rightBuffer;
	};
}