#pragma once

#ifdef _DLL_EXPORT
#	define EZMP_DLL __declspec(dllexport)
#else
#	ifndef _LIB
#		define EZMP_DLL __declspec(dllimport)
#	else
#		define EZMP_DLL
#	endif
#endif