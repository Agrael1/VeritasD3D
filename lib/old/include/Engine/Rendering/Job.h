#pragma once

class Drawable;
class Graphics;
class Step;

namespace RG
{
	class Job
	{
	public:
		Job(const Step* pStep, const Drawable* pDrawable);
		void Execute(Graphics& gfx) const noxnd;
	private:
		const Drawable* pDrawable;
		const Step* pStep;
	};
}