#pragma once
#include <Framework/noexcept_if.h>

class Job
{
public:
	Job(const class Step* pStep, const class Drawable* pDrawable);
	void Execute(class Graphics& gfx) const noxnd;
private:
	const class Drawable* pDrawable;
	const class Step* pStep;
};