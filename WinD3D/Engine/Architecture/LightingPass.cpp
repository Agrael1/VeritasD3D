#include "LightingPass.h"
#include "DepthStencil.h"
#include "SourcesT.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "FullscreenEffect.h"
#include "Sampler.h"
#include "IndexBuffer2.h"

RG::LightingPass::LightingPass(Graphics& gfx, std::string name) noexcept(!IS_DEBUG)
	:BindingPass(std::move(name))
{
	AddBindSink<RenderTargetArray>("targets");
	RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
	RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));

	AddBind(std::make_shared<ver::FullscreenEffect>());
	AddBind(Sampler::Resolve(gfx));
	AddBind(std::make_shared<ver::IndexBuffer>(gfx, "$$full", std::array< const uint16_t, 3>{ 0, 1, 2 }));
	AddBind(std::move(VertexShader::Resolve(gfx, "fullscreen.vs.cso")));
	AddBind(std::move(PixelShader::Resolve(gfx, "fullscreen.ps.cso")));
}
void RG::LightingPass::Execute(Graphics& gfx) const noxnd
{
	BindAll(gfx);
	gfx.DrawIndexed(3u);
}