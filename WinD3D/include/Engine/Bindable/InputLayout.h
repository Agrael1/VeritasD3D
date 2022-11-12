#pragma once
#include <Engine/Bindable/Bindable.h>
#include <Engine/Dynamic/VertexLayout.h>

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx,
		ver::dv::LayoutSpan layout,
		ID3DBlob* pVertexShaderBytecode, bool multi = false);
public:
	void Bind(Graphics& gfx) noxnd override;
	std::string GetUID()const noexcept override;
public:
	static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
		ver::dv::LayoutSpan layout, ID3DBlob* pVertexShaderBytecode, bool multi = false);
	static std::string GenerateUID(ver::dv::LayoutSpan layout, ID3DBlob* pVertexShaderBytecode = nullptr, bool multi = false);
protected:
	std::string tag;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};
