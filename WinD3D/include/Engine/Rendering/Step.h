#pragma once
#include <vector>
#include <memory>

class TechniqueProbe;
class Drawable;
class Graphics;
class Bindable;

namespace RG
{
	class RenderQueuePass;
	class RenderGraph;
}

class Step
{
public:
	Step(std::string targetPassName);
	Step(Step&&) = default;
	Step(const Step & src) noexcept;
	Step& operator=(const Step&) = delete;
	Step& operator=(Step&&) = delete;
public:
	void AddBindable(std::shared_ptr<Bindable> bind_in) noexcept;
	void Submit(const Drawable & drawable) const;
	void Bind(Graphics & gfx) const noxnd;
	void InitializeParentReferences(const Drawable & parent) noexcept;
	void Accept(TechniqueProbe& probe);
	void Link(RG::RenderGraph& rg);
	void Unlink();
private:
	std::vector<std::shared_ptr<Bindable>> bindables;
	RG::RenderQueuePass* pTargetPass = nullptr;
	std::string targetPassName;
};