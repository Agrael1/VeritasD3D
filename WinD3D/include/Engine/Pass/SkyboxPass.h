#pragma once 
#include <Engine/Pass/BindingPass.h>
#include <Engine/Bindable/CubeTexture.h>
#include <Engine/Bindable/BindableCommons.h>

class Graphics;

using namespace RG;

namespace ver::rg
{
	class SkyboxPass : public ::RG::BindingPass
	{
	public:
		SkyboxPass(Graphics& gfx, std::string name)
			:
			BindingPass(std::move(name))
		{
			constexpr uint16_t a[]{ 3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0 };
			RegisterSink(DirectBufferSink<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(std::make_shared<CubeTexture>(gfx, L"../assets/face.dds"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
			AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear));
			AddBind(IndexBuffer::Resolve(gfx, "$GlobCube", a));
			AddBind(std::make_shared<SkyboxTransformCbuf>(gfx));
			AddBind(PixelShader::Resolve(gfx, "skybox.ps.cso"));
			AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP));
			AddBind(VertexShader::Resolve(gfx, "skybox.vs.cso"));
			RegisterSource(DirectBufferSource<IRenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
		}
	public:
		void Execute(Graphics& gfx) const noxnd override
		{
			BindAll(gfx);
			gfx.DrawIndexed(14u);
		}
	};
}