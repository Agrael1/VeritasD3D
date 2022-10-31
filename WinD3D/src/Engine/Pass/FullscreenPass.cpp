#include <Engine/Pass/FullscreenPass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Graphics.h>

namespace RG
{
	namespace dx = DirectX;

	FullscreenPass::FullscreenPass(const std::string name, Graphics& gfx) noxnd
		:
	BindingPass(std::move(name))
	{
		// setup fullscreen geometry
		DV::VertexLayout lay;
		lay
			+ DV::Type::Position2D;
		DV::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });
		AddBind(VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull)));
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		AddBind(IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));
		// setup other common fullscreen bindables
		auto vs = VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		AddBind(InputLayout::Resolve(gfx, lay, vs->GetBytecode()));
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