#include "LightingPass.h"
#include "DepthStencil.h"
#include "Graphics.h"
#include "SourcesT.h"
#include "VertexShader.h"
#include "RasterizerState.h"
#include "Topology.h"
#include "PixelShader.h"

RG::LightingPass::LightingPass(Graphics& gfx, std::string name) noexcept(!IS_DEBUG)
	:BindingPass(std::move(name))
{
	RegisterSink(DirectBufferSink<RenderTargetArray>::Make("targets", rts));
	RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
	RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));

	AddBind(std::move(VertexShader::Resolve(gfx, "fullscreen.vs.cso")));
	AddBind(Topology::Resolve(gfx));
	AddBind(RasterizerState::Resolve(gfx, false));
	AddBind(std::move(PixelShader::Resolve(gfx, "fullscreen.ps.cso")));

}
void RG::LightingPass::Execute(Graphics& gfx) const noxnd
{
	BindAll(gfx);
	gfx.DrawIndexed(3u);
}