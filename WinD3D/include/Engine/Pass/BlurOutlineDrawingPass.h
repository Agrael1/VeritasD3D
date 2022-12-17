#pragma once
#include "RenderQueuePass.h"
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/PixelShader.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Bindable/BlendState.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Rendering/SourcesT.h>

namespace RG
{
	class BlurOutlineDrawingPass : public RenderQueuePass
	{
	public:
		BlurOutlineDrawingPass(Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight)
			:RenderQueuePass(std::move(name))
		{
			renderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0);
			AddBind(ver::VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(ver::PixelShader::Resolve(gfx, "Solid_PS.cso"));
			AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Mask));
			AddBind(ver::BlendState::Resolve(gfx, false));
			RegisterSource(DirectBindableSource<IRenderTarget>::Make("scratchOut", renderTarget));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			renderTarget->Clear(gfx);
			RenderQueuePass::Execute(gfx);
		}
	};
}