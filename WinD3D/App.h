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
	void DoFrame();
private:
	Window wnd;
	bool bDirectInput;
};

