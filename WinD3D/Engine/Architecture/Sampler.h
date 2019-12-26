#pragma once
#include <Engine/Architecture/Bindable.h>
#include <memory> 

class Sampler : public Bindable
{
public:
	Sampler(Graphics& gfx);
public:
	void Bind(Graphics& gfx)noexcept override;
	std::string GetUID() const noexcept override;
public:
	static std::shared_ptr<Sampler> Resolve(Graphics& gfx);
	static std::string GenerateUID();
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
};