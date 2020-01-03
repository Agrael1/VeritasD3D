#pragma once
#include <Engine/Architecture/Bindable.h>
#include <memory>


class BlendState : public Bindable
{
public:
	BlendState(Graphics& gfx, bool blending);
public:
	void Bind(Graphics& gfx)noexcept override;
	std::string GetUID() const noexcept override;
public:
	static std::shared_ptr<BlendState> Resolve(Graphics& gfx, bool blending);
	static std::string GenerateUID(bool Blending);
protected:
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
	bool blendingMode;
};