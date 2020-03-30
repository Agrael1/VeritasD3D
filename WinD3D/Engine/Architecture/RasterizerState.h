#pragma once
#include <Engine/Architecture/Bindable.h>

class RasterizerState : public Bindable
{
public:
	RasterizerState(Graphics& gfx, bool twosided);
public:
	void Bind(Graphics& gfx)noxnd override;
	std::string GetUID() const noexcept override;
public:
	static std::shared_ptr<RasterizerState> Resolve(Graphics& gfx, bool twosided);
	static std::string GenerateUID(bool twoSided);
protected:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	bool twoSided;
};