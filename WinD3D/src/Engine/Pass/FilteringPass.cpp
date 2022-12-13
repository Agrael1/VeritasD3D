#include <Engine/Pass/FilteringPass.h>
#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/PixelShader.h>
#include <Engine/Bindable/Sampler.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Graphics.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/Topology.h>

ver::rg::FilteringPass::FilteringPass(Graphics& gfx, std::string name) noxnd
	:RG::BindingPass(std::move(name))
{
	pxc = std::make_shared<decltype(pxc)::element_type>(gfx, DirectX::XMFLOAT2{ float(gfx.GetWidth()), float(gfx.GetHeight()) },0);
	AddBindSink<IRenderTarget>("preFilterTarget");
	RegisterSink(RG::DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
	RegisterSource(RG::DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));

	AddBind(Sampler::Resolve(gfx));

	AddBind(ver::VertexShader::Resolve(gfx, "fullscreen.vs.cso"));
	AddBind(ver::PixelShader::Resolve(gfx, "filter.ps.cso"));
	AddBind(pxc);

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
}
void ver::rg::FilteringPass::Execute(Graphics& gfx) const noxnd
{
	pxc->Update(gfx, DirectX::XMFLOAT2{ float(gfx.GetWidth()), float(gfx.GetHeight()) });
	renderTarget->BindAsBuffer(gfx);
	for (auto& bind : binds)
	{
		bind->Bind(gfx);
	}
	gfx.Draw(3u);
}
