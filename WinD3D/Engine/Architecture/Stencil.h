#pragma once
#include <Engine/Architecture/Bindable.h>
#include <memory> 

class Stencil : public Bindable
{
public:
	enum class Mode
	{
		Off,
		Write,
		Mask
	};
public:
	Stencil(Graphics& gfx, Mode mode);
public:
	void Bind(Graphics& gfx)noexcept override;
	std::string GetUID() const noexcept override;
public:
	static std::shared_ptr<Stencil> Resolve(Graphics& gfx, Mode mode);
	static std::string GenerateUID(Mode mode);
protected:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
	Mode mode;
};