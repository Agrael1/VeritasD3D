#include <Engine/Pass/HorizontalBlurPass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Bindable/RenderTarget.h>

namespace RG
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight)
		:
		FullscreenPass(std::move(name), gfx)
	{
		AddBind(ver::PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(BlendState::Resolve(gfx, false));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Point, true));

		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", direction));

		// the renderTarget is internally sourced and then exported as a Bindable
		renderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		RegisterSource(DirectBindableSource<IRenderTarget>::Make("scratchOut", renderTarget));
	}

	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)
	void HorizontalBlurPass::Execute(Graphics& gfx) const noxnd
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}
