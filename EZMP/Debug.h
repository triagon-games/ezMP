#pragma once
#include "pch.h"
#include "CommonDefinitions.h"
#include <wtypes.h>

namespace DEBUG
{
	EZMP_DLL void printVerboseDebug(std::string p, uint8_t verbosityLevel)
	{
#	ifdef _DEBUG
		if (DEBUG_VERBOSITY_LEVEL >= verbosityLevel)
		{
			std::printf(p.c_str());
		}
#	endif
	}
}