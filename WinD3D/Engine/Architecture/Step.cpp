#include "Step.h"
#include "FrameCommander.h"

void Step::AddBindable(std::shared_ptr<Bindable> bind_in) noexcept
{
	bindables.push_back(std::move(bind_in));
}

void Step::Submit(FrameCommander& frame, const Drawable& drawable) const
{
	frame.Accept(Job{ this, &drawable }, targetPass);
}
void Step::Bind(Graphics& gfx) const
{
	for (const auto& b : bindables)
	{
		b->Bind(gfx);
	}
}
void Step::InitializeParentReferences(const Drawable& parent) noexcept
{
	for ( auto& b : bindables)
	{
		b->InitializeParentReference(parent);
	}
}
void Step::Accept(TechniqueProbe& probe)
{
	probe.SetStep(this);
	for (auto& pb : bindables)
	{
		pb->Accept(probe);
	}
}
