#pragma once
#include "Engine\Window.h"

class App
{
public:
	App();
	~App();
public:
	int Go();
private:
	void DoFrame(float dt);
private:
	Window wnd;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};

