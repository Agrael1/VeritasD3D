#pragma once
#include "RenderQueuePass.h"
#include "RenderTarget.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Stencil.h"
#include "BlendState.h"
#include "RenderTarget.h"
#include "SourcesT.h"

namespace RG
{
	class BlurOutlineDrawingPass : public RenderQueuePass
	{
	public:
		BlurOutlineDrawingPass(Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight)
			:RenderQueuePass(std::move(name))
		{
			renderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0);
			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
			AddBind(BlendState::Resolve(gfx, false));
			RegisterSource(DirectBindableSource<RenderTarget>::Make("scratchOut", renderTarget));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			renderTarget->Clear(gfx);
			RenderQueuePass::Execute(gfx);
		}
	};
}