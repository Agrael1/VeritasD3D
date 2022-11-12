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
		AddBind(BlendState::Resolve(gfx, true));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear, true));

		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", direction));
		RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
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