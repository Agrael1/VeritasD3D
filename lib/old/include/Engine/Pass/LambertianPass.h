#pragma once
#include <Engine/Pass/RenderQueuePass.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Graphics.h>

namespace RG
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBindableSource<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Off));
		}
	};
}

namespace ver::rg
{
	class StereoLambertianPass : public RG::RenderQueuePass
	{
	public:
		StereoLambertianPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
		{
			RegisterSink(RG::DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(RG::DirectBufferSink<IRenderTarget>::Make("renderTarget2", renderTarget2));
			RegisterSink(RG::DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSink(RG::DirectBufferSink<ver::DepthStencil>::Make("depthStencil2", depthStencil2));


			RegisterSource(RG::DirectBindableSource<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(RG::DirectBindableSource<IRenderTarget>::Make("renderTarget2", renderTarget2));

			RegisterSource(RG::DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(RG::DirectBufferSource<ver::DepthStencil>::Make("depthStencil2", depthStencil2));
			AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Off));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			RenderQueuePass::Execute(gfx);
			std::swap(renderTarget, renderTarget2);
			std::swap(depthStencil, depthStencil2);

			//camera
			gfx.SetCamera(Graphics::right);

			RenderQueuePass::Execute(gfx);
			std::swap(renderTarget, renderTarget2);
			std::swap(depthStencil, depthStencil2);

			gfx.SetCamera(Graphics::left);
		}
	private:
		mutable std::shared_ptr<IRenderTarget> renderTarget2;
		mutable std::shared_ptr<ver::DepthStencil> depthStencil2;
	};
}