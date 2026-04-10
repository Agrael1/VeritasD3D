#pragma once
#include <Engine/Pass/BindingPass.h>
#include <Engine/Bindable/GraphicsResource.h>

namespace RG
{
	class LightingPass : public BindingPass, public ::ver::GraphicsResource
	{
	public:
		LightingPass(Graphics& gfx, std::string name, size_t slot = 0) noxnd;
		void Execute(Graphics& gfx) const noxnd override;
	};
}