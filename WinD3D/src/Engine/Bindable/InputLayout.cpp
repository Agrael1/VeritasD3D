#include <Engine/Bindable/InputLayout.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

InputLayout::InputLayout(Graphics & gfx, DV::VertexLayout layout_in, ID3DBlob * pVertexShaderBytecode, bool multi)
	:layout(std::move(layout_in))
{
	INFOMAN(gfx);

	const auto d3dlayout = multi? layout.GetD3DMultilayout():layout.GetD3DLayout();

	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
		d3dlayout.data(), (UINT)d3dlayout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	));
}

void InputLayout::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
}
std::string InputLayout::GetUID() const noexcept
{
	return GenerateUID(layout);
}

const DV::VertexLayout InputLayout::GetLayout() const noexcept
{
	return layout;
}

std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
	const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode, bool multi)
{
	return ver::Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderBytecode, multi);;
}

std::string InputLayout::GenerateUID(const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode, bool multi)
{
	using namespace std::string_literals;
	return typeid(InputLayout).name() + "#"s + layout.GetCode() + std::to_string(multi);
}

