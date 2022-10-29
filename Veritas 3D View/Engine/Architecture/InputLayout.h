#pragma once
#include "Bindable.h"
#include "VertexLayout.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx,
		DV::VertexLayout layout,
		ID3DBlob* pVertexShaderBytecode);
public:
	void Bind(Graphics& gfx) noxnd override;
	std::string GetUID()const noexcept override;
	const DV::VertexLayout GetLayout() const noexcept;
public:
	static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
		const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode);
	static std::string GenerateUID(const DV::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);
protected:
	DV::VertexLayout layout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};