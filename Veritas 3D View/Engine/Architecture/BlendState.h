#pragma once
#include "Bindable.h"
#include <optional>
#include <array>


class BlendState : public Bindable
{
public:
	BlendState(Graphics& gfx, bool blending, std::optional<float> factor = {});
	void Bind(Graphics& gfx) noxnd override;
	void SetFactor(float factor) noxnd;
	float GetFactor() const noxnd;
	static std::shared_ptr<BlendState> Resolve(Graphics& gfx, bool blending, std::optional<float> factor = {});
	static std::string GenerateUID(bool blending, std::optional<float> factor);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
	bool blending;
	std::optional<std::array<float, 4>> factors;
};