#pragma once
#include <Engine/Pass/RenderQueuePass.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Rendering/SourcesT.h>

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