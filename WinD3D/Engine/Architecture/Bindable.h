#pragma once
#include <Engine/Graphics.h>

class Bindable
{
public:
	virtual ~Bindable() = default;
public:
	virtual void Bind(Graphics& gfx)noexcept = 0;
	virtual void InitializeParentReference(const class Drawable&)noexcept
	{

	}
	virtual void Accept(class TechniqueProbe&)
	{

	}
	virtual std::string GetUID() const noexcept
	{
		assert(false);
		return "";
	}
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx)noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx)noexcept;
	static DXGIInfoManager& GetInfoManager(Graphics& gfx)noexcept(IS_DEBUG);
};

