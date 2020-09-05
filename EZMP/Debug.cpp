#include "Debug.h"

namespace DEBUG
{
	void printVerboseDebug(std::string p, uint8_t verbosityLevel)
	{
#	ifdef _DEBUG
		if (DEBUG_VERBOSITY_LEVEL >= verbosityLevel)
		{
			std::printf(p.c_str());
		}
#	endif
	}
}