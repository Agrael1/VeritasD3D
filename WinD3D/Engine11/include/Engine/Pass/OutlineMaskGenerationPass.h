#pragma once
#include "RenderQueuePass.h"
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Bindable/NullPixelShader.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/RasterizerState.h>


namespace RG
{
	class OutlineMaskGenerationPass : public RenderQueuePass
	{
	public:
		OutlineMaskGenerationPass(Graphics& gfx, std::string name)
			:RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(ver::VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::Write));
			AddBind(RasterizerState::Resolve(gfx, false));
		}
	};
}