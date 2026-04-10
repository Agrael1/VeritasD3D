#include <Engine/Pass/VerticalBlurPass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Rendering/SourcesT.h>

namespace RG
{
	VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& gfx)
		:
		FullscreenPass(std::move(name), gfx)
	{
		AddBind(ver::PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(ver::BlendState::Resolve(gfx, true));
		AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Mask));
		AddBind(ver::Sampler::Resolve(gfx, ver::Sampler::Type::Bilinear, true));

		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", direction));
		RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute(Graphics& gfx) const noxnd
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}