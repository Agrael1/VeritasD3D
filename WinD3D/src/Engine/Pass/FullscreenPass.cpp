#include <Engine/Pass/FullscreenPass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Graphics.h>

namespace RG
{
	namespace dx = DirectX;

	FullscreenPass::FullscreenPass(std::string name, Graphics& gfx) noxnd
		:
	BindingPass(std::move(name))
	{
		// setup fullscreen geometry
		ver::dv::VertexLayout lay{ ver::dv::ElementType::Position2D };
		ver::dv::VertexBuffer bufFull{ std::move(lay) };
		bufFull.emplace_back(dx::XMFLOAT2{ -1,1 });
		bufFull.emplace_back(dx::XMFLOAT2{ 1,1 });
		bufFull.emplace_back(dx::XMFLOAT2{ -1,-1 });
		bufFull.emplace_back(dx::XMFLOAT2{ 1,-1 });

		AddBind(VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull)));
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		AddBind(ver::IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));
		// setup other common fullscreen bindables
		auto vs = ver::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		AddBind(InputLayout::Resolve(gfx, bufFull.layout(), vs->GetBytecode()));
		AddBind(std::move(vs));
		AddBind(Topology::Resolve(gfx));
		AddBind(RasterizerState::Resolve(gfx, false));
	}

	void FullscreenPass::Execute(Graphics& gfx) const noxnd
	{
		BindAll(gfx);
		gfx.DrawIndexed(6u);
	}
}