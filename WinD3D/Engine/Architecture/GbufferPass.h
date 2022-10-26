#pragma once
#include "RenderQueuePass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "SinksT.h"
#include "SourcesT.h"
#include "Stencil.h"


namespace RG
{
	class GBufferPass : public RenderQueuePass
	{
	public:
		GBufferPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
		{
			auto rts = std::make_shared<RenderTargetArray>();
			rts->InitializeAsync(gfx, gfx.GetWidth(), gfx.GetHeight(), 0).get();
			renderTarget = rts;

			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<RenderTargetArray>::Make("targets", (std::shared_ptr<RenderTargetArray>&)(renderTarget)));

			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
	};
}

