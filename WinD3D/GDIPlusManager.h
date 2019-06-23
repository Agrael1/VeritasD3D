#pragma once
#define FULL_WINOPT
#include "Engine\WinSetup.h"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};