#pragma once
#include <Engine/Bindable/GraphicsResource.h>
#include <string>
#include <memory>

class Bindable : public ver::GraphicsResource
{
public:
	virtual ~Bindable() = default;
public:
	virtual void Bind(Graphics& gfx)noxnd = 0;
	virtual void Bind(ID3D11DeviceContext& context)noxnd {}
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
};

class CloningBindable : public Bindable
{
public:
	virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
};