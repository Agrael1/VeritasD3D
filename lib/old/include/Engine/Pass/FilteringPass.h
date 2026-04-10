#pragma once
#include <Engine/Pass/BindingPass.h>
#include <Engine/Bindable/ConstantBuffer.h>


namespace ver::rg
{
	class FilteringPass : public RG::BindingPass, public GraphicsResource
	{
	public:
		FilteringPass(Graphics& gfx, std::string name) noxnd;
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<PixelConstantBuffer<DirectX::XMFLOAT2>> pxc;
	};
}