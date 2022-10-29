#pragma once
#include <Engine\Architecture\GraphicsResource.h>
#include <Framework\noexcept_if.h>
#include <memory>

class Bindable : public GraphicsResource
{
public:
	virtual ~Bindable() = default;
public:
	virtual void Bind(Graphics& gfx)noxnd = 0;
	virtual void InitializeParentReference(const class Drawable&)noexcept
	{

	}
	virtual void Accept(class TechniqueProbe&)
	{

	}
	virtual std::string GetUID() const noexcept;
};

class CloningBindable : public Bindable
{
public:
	virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
};