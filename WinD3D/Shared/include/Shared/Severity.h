#pragma once
#ifndef MODULE
#include <Shared/Defines.h>
#endif

SHARED_EXPORT namespace ver
{
	enum class Severity
	{
		debug,
		trace,
		info,
		warn,
		error,
		critical
	};
}
