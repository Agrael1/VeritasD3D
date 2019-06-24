#pragma once
#include "Bindable.h"
#include "ReSurface.h"

class Texture :public Bindable
{
public:
	Texture(Graphics& gfx, ReSurface& s);
	void Bind(Graphics& gfx)noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};