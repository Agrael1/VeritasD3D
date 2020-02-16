#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include <Engine/Graphics.h>
#include "TechniqueProbe.h"

class Step
{
public:
	Step(size_t targetPass_in)
		:
		targetPass{ targetPass_in }
	{}
	Step(Step&&) = default;
	//Step(const Step& src) noexcept
	//	:
	//	targetPass(src.targetPass)
	//{
	//	bindables.reserve(src.bindables.size());
	//	for (auto& pb : src.bindables)
	//	{
	//		if (auto* pCloning = dynamic_cast<const CloningBindable*>(pb.get()))
	//		{
	//			bindables.push_back(pCloning->Clone());
	//		}
	//		else
	//		{
	//			bindables.push_back(pb);
	//		}
	//	}
	//}
	Step& operator=(const Step&) = delete;
	Step& operator=(Step&&) = delete;
public:
	void AddBindable(std::shared_ptr<Bindable> bind_in) noexcept;
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const;
	void Bind(Graphics& gfx) const;
	void InitializeParentReferences(const class Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bindable>> bindables;
};