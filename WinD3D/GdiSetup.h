#pragma once
#define FULL_WINOPT
#include "Engine\WinSetup.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>

#pragma comment( lib,"gdiplus.lib" )