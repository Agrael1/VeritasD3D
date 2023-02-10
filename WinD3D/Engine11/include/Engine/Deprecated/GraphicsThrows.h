#pragma once
#include <Engine/Util/GraphicsExceptions.h>

#define GFX_THROW_NOINFO(hrcall) ver::check_hresult(hrcall)
#define GFX_THROW_INFO(hrcall) ver::check_graphics(hrcall)
#define GFX_THROW_INFO_ONLY(call) call; ver::check_context();

#define INFOMAN_NOHR(gfx)
#define INFOMAN(gfx)