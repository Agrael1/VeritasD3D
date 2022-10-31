#pragma once
#include <Engine/Bindable/Bindable.h>
#include <Engine/Dynamic/VertexLayout.h>

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx,
		DV::VertexLayout layout,
		ID3DBlob* pVertexShaderBytecode, bool multi = false);
public:
	void Bind(Graphics& gfx) noxnd override;
	std::string GetUID()const noexcept override;
	const DV::VertexLayout GetLayout() const noexcept;
public:
	static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
		const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode, bool multi = false);
	static std::string GenerateUID(const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr, bool multi = false);
protected:
	DV::VertexLayout layout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};
