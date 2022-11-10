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
	class GBufferPass : public RenderQueuePass, public ver::GraphicsResource
	{
	public:
		GBufferPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
		{
			auto rts = std::make_shared<RenderTargetArray>();
			rts->InitializeAsync(gfx, gfx.GetWidth(), gfx.GetHeight(), 0).get();
			renderTarget = rts;

			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBindableSource<RenderTargetArray>::Make("targets", (std::shared_ptr<RenderTargetArray>&)(renderTarget)));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			((std::shared_ptr<RenderTargetArray>&)(renderTarget))->Clear(gfx);
			RenderQueuePass::Execute(gfx);
		}
	};
}
