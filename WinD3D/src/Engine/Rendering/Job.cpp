#include <Engine/Rendering/Job.h>
#include <Engine/Rendering/Step.h>
#include <Engine/Rendering/Drawable.h>
#include <Engine/Graphics.h>

namespace RG
{
	Job::Job(const Step* pStep, const Drawable* pDrawable)
		:
		pDrawable{ pDrawable },
		pStep{ pStep }
	{
	}

	void Job::Execute(Graphics& gfx) const noxnd
	{
		pDrawable->Bind(gfx);
		pStep->Bind(gfx);
		pDrawable->Draw(gfx);
	}
}