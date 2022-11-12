#include <Engine/Bindable/InputLayout.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

InputLayout::InputLayout(Graphics & gfx, ver::dv::LayoutSpan layout, ID3DBlob * pVertexShaderBytecode, bool multi)
	:tag(GenerateUID(layout, nullptr, multi))
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
	return tag;
}

std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
	ver::dv::LayoutSpan layout, ID3DBlob* pVertexShaderBytecode, bool multi)
{
	return ver::Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderBytecode, multi);
}

std::string InputLayout::GenerateUID(ver::dv::LayoutSpan layout, ID3DBlob* pVertexShaderBytecode, bool multi)
{
	using namespace std::string_literals;
	return typeid(InputLayout).name() + "#"s + layout.code() + std::to_string(multi);
}

