#include <Engine/Bindable/Stencil.h>
#include <Engine/Bindable/Codex.h>
#include <Shared/Checks.h>
#include <format>

ver::Stencil::Stencil(Graphics& gfx, Mode mode)
	:mode(mode)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

	if (mode == Mode::Write)
	{
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	else if (mode == Mode::Mask)
	{
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	}
	else if (mode == Mode::DepthFirst)
	{
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	GetDevice(gfx)->CreateDepthStencilState(&dsDesc, pStencil.put());
}

void ver::Stencil::Bind(Graphics& gfx) noxnd
{
	Bind(*GetContext(gfx));
}
void ver::Stencil::Bind(ID3D11DeviceContext& context) noxnd
{
	context.OMSetDepthStencilState(pStencil.get(), 0xFF);
	ver::check_context();
}

std::shared_ptr<ver::Stencil> ver::Stencil::Resolve(Graphics& gfx, Mode mode)
{
	return ver::Codex::Resolve<ver::Stencil>(gfx, mode);
}
std::string ver::Stencil::GenerateUID(Mode mode)
{
	using namespace std::string_literals;
	const auto modeName = [mode]() {
		switch (mode) {
		case Mode::Off:
			return "off"s;
		case Mode::Write:
			return "write"s;
		case Mode::Mask:
			return "mask"s;
		case Mode::DepthFirst:
			return "depth-first"s;
		}
		return "ERROR"s;
	};
	return std::format("Stencil#{}", modeName());
}
std::string ver::Stencil::GetUID() const noexcept
{
	return GenerateUID(mode);
}
