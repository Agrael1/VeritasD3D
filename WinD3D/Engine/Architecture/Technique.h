#pragma once
#include "Step.h"
#include <vector>


class TechniqueProbe;
class Drawable;

namespace RG
{
	class RenderGraph;
}

class Technique
{
public:
	Technique() = default;
	Technique(std::string name, bool startActive = true) noexcept;
public:
	void Submit(const Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState(bool active_in) noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const noexcept;
	void Link(RG::RenderGraph&);
	void Unlink();
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name;
};