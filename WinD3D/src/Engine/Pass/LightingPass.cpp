#include <Engine/Pass/LightingPass.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/PixelShader.h>
#include <Engine/Bindable/Sampler.h>
#include <Engine/Bindable/IndexBuffer2.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Graphics.h>
#include <Engine/Bindable/RenderTarget.h>

RG::LightingPass::LightingPass(Graphics& gfx, std::string name) noxnd
	:BindingPass(std::move(name))
{
	AddBindSink<RenderTargetArray>("targets");
	RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
	RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));

	AddBind(Sampler::Resolve(gfx));
	AddBind(std::make_shared<ver::IndexBuffer>(gfx, "$$full", std::array< const uint16_t, 3>{ 0, 1, 2 }));
	AddBind(std::move(VertexShader::Resolve(gfx, "fullscreen.vs.cso")));
	AddBind(std::move(PixelShader::Resolve(gfx, "fullscreen.ps.cso")));
}
void RG::LightingPass::Execute(Graphics& gfx) const noxnd
{
	BindAll(gfx);
	gfx.DrawIndexed(3u);
	ID3D11ShaderResourceView* const pSRV[4] = { nullptr };
	ver::GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 4, pSRV);
}
