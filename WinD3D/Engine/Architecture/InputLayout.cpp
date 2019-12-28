#include "InputLayout.h"
#include "GraphicsThrows.m"
#include <Engine/Architecture/Codex.h>

InputLayout::InputLayout(Graphics & gfx, DV::VertexLayout layout, ID3DBlob * pVertexShaderBytecode)
{
	INFOMAN(gfx);

	const auto d3dlayout = layout.GetD3DLayout();

	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
		d3dlayout.data(), (UINT)d3dlayout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	));
}

void InputLayout::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}
std::string InputLayout::GetUID() const noexcept
{
	return GenerateUID(layout);
}

std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
	const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode)
{
	return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderBytecode);;
}

std::string InputLayout::GenerateUID(const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode)
{
	using namespace std::string_literals;
	return typeid(InputLayout).name() + "#"s + layout.GetCode();
}
