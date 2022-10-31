#pragma once
#include "Bindable.h"
#include <memory>

class NullPixelShader : public Bindable
{
public:
	NullPixelShader(Graphics& gfx);
public:
	void Bind(Graphics& gfx) noxnd override;
	static std::shared_ptr<NullPixelShader> Resolve(Graphics& gfx);
	static std::string GenerateUID();
	std::string GetUID() const noexcept override;
};