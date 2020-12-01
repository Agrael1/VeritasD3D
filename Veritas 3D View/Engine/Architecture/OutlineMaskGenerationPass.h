#pragma once
#include "RenderQueuePass.h"
#include "SourcesT.h"
#include "SinksT.h"
#include "NullPixelShader.h"
#include "Stencil.h"
#include "VertexShader.h"
#include "RasterizerState.h"


namespace RG
{
	class OutlineMaskGenerationPass : public RenderQueuePass
	{
	public:
		OutlineMaskGenerationPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Write));
			AddBind(RasterizerState::Resolve(gfx, false));
		}
	};
}