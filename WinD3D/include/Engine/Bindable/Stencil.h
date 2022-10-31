#pragma once
#include <Engine/Bindable/Bindable.h>
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
	Stencil(Graphics& gfx, Mode mode);
public:
	void Bind(Graphics& gfx) noxnd override;
	static std::shared_ptr<Stencil> Resolve(Graphics& gfx, Mode mode);
	static std::string GenerateUID(Mode mode);
	std::string GetUID() const noexcept override;
private:
	Mode mode;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
};