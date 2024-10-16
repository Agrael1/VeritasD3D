#include <Engine/Pass/LightingPass.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/PixelShader.h>
#include <Engine/Bindable/Sampler.h>
#include <Engine/Bindable/IndexBuffer.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Graphics.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/Topology.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Bindable/ShadowSampler.h>

RG::LightingPass::LightingPass(Graphics& gfx, std::string name, size_t slot) noxnd
	:BindingPass(std::move(name))
{
	AddBindSink<RenderTargetArray>("targets");
	renderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight(), slot); //pre filter rt
	RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
	RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));

	AddBindSink<Bindable>("shadow");

	AddBind(std::make_shared<ver::ShadowSampler>(gfx));
	AddBind(ver::Stencil::Resolve(gfx, ver::Stencil::Mode::DepthFirst));
	AddBind(ver::Sampler::Resolve(gfx));
	AddBind(ver::VertexShader::Resolve(gfx, "fullscreen.vs.cso"));
	AddBind(ver::PixelShader::Resolve(gfx, "fullscreen.ps.cso"));
}
void RG::LightingPass::Execute(Graphics& gfx) const noxnd
{
	ver::Topology::Bind(gfx);
	BindAll(gfx);
	gfx.Draw(3u);
	ID3D11ShaderResourceView* const pSRV[6] = { nullptr };
	ver::GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 6, pSRV);
}
