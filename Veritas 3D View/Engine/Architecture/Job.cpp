#include "pch.h"
#include "Job.h"
#include "Step.h"
#include "Drawable.h"

namespace RG
{
	Job::Job(const Step* pStep, const Drawable* pDrawable)
		:
		pDrawable{ pDrawable },
		pStep{ pStep }
	{
	}

	void Job::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		pDrawable->Bind(gfx);
		pStep->Bind(gfx);
		gfx.DrawIndexed(pDrawable->GetIndexCount());
	}
}