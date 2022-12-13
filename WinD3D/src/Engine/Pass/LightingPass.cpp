#include <Engine/Pass/LightingPass.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/PixelShader.h>
#include <Engine/Bindable/Sampler.h>
#include <Engine/Bindable/IndexBuffer2.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Graphics.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/Topology.h>
#include <Engine/Bindable/Stencil.h>
#include <Engine/Bindable/ShadowSampler.h>

RG::LightingPass::LightingPass(Graphics& gfx, std::string name) noxnd
	:BindingPass(std::move(name))
{
	AddBindSink<RenderTargetArray>("targets");
	renderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight(), 0); //pre filter rt
	RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
	RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));

	AddBindSink<Bindable>("shadow");

	AddBind(std::make_shared<ver::ShadowSampler>(gfx));
	AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
	AddBind(Sampler::Resolve(gfx));
	AddBind(ver::VertexShader::Resolve(gfx, "fullscreen.vs.cso"));
	AddBind(ver::PixelShader::Resolve(gfx, "fullscreen.ps.cso"));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
}
void RG::LightingPass::Execute(Graphics& gfx) const noxnd
{
	BindAll(gfx);
	gfx.Draw(3u);
	ID3D11ShaderResourceView* const pSRV[6] = { nullptr };
	ver::GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 6, pSRV);
}
