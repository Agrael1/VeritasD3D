#pragma once
#include <Engine/Pass/RenderQueuePass.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/TransformCBuf.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Graphics.h>


namespace RG
{
	class GBufferPass : public RenderQueuePass, public ver::GraphicsResource
	{
	public:
		GBufferPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
			,pShadowCBuf{ std::make_shared<ver::ShadowCameraCBuf>(gfx) }
		{
			AddBind(pShadowCBuf);
			auto rts = std::make_shared<RenderTargetArray>();
			rts->InitializeAsync(gfx, gfx.GetWidth(), gfx.GetHeight(), 0).get();
			renderTarget = rts;
			AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Off));
			RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBindableSource<RenderTargetArray>::Make("targets", (std::shared_ptr<RenderTargetArray>&)(renderTarget)));
			RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			((std::shared_ptr<RenderTargetArray>&)(renderTarget))->Clear(gfx);
			pShadowCBuf->Update(gfx);
			RenderQueuePass::Execute(gfx);
		}
	private:
		std::shared_ptr<ver::ShadowCameraCBuf> pShadowCBuf;
	};


	class StereoGBufferPass : public RenderQueuePass, public ver::GraphicsResource
	{
	public:
		StereoGBufferPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
			, pShadowCBuf{ std::make_shared<ver::ShadowCameraCBuf>(gfx) }
		{
			AddBind(pShadowCBuf);
			auto rts = std::make_shared<RenderTargetArray>();
			rts->InitializeAsync(gfx, gfx.GetWidth(), gfx.GetHeight(), 0).get();
			auxBuffers = std::make_shared<RenderTargetArray>();
			auxBuffers->InitializeAsync(gfx, gfx.GetWidth(), gfx.GetHeight(), 0).get();

			renderTarget = rts;

			AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Off));
			RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("auxDepth", auxDepth));

			RegisterSource(DirectBindableSource<RenderTargetArray>::Make("targets", (std::shared_ptr<RenderTargetArray>&)(renderTarget)));
			RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));

			RegisterSource(DirectBindableSource<RenderTargetArray>::Make("auxTargets", auxBuffers));
			RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("auxDepth", auxDepth));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			((std::shared_ptr<RenderTargetArray>&)(renderTarget))->Clear(gfx);
			pShadowCBuf->Update(gfx);
			RenderQueuePass::Execute(gfx);

			std::swap(auxBuffers, (std::shared_ptr<RenderTargetArray>&)renderTarget);
			std::swap(auxDepth, depthStencil);

			//change camera
			gfx.SetCamera(false);

			((std::shared_ptr<RenderTargetArray>&)(renderTarget))->Clear(gfx);
			pShadowCBuf->Update(gfx);
			RenderQueuePass::Execute(gfx);

			std::swap(auxBuffers, (std::shared_ptr<RenderTargetArray>&)renderTarget);
			std::swap(auxDepth, depthStencil);

			gfx.SetCamera(true);
		}
	private:
		std::shared_ptr<ver::ShadowCameraCBuf> pShadowCBuf;
		mutable std::shared_ptr<RenderTargetArray> auxBuffers;
		mutable std::shared_ptr<ver::DepthStencil> auxDepth;
	};
}

