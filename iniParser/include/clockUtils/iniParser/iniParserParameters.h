/**
 * \addtogroup iniParser
 * @{
 */

#ifndef __CLOCKUTILS_INIPARSER_INIPARSERPARAMETERS_H__
#define __CLOCKUTILS_INIPARSER_INIPARSERPARAMETERS_H__

#include "clockUtils/SystemParameters.h"

// Dynamic library import/export macro
#ifndef CLOCK_INIPARSER_API
	#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		#ifdef CLOCKUTILS_BUILD_SHARED
			#ifdef clock_iniParser_EXPORTS
				#define CLOCK_INIPARSER_API __declspec(dllexport)
			#else
				#define CLOCK_INIPARSER_API __declspec(dllimport)
			#endif
		#else
			#define CLOCK_INIPARSER_API
		#endif
	#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		#define CLOCK_INIPARSER_API
	#else
		#define CLOCK_INIPARSER_API
	#endif
#endif

/**
 * @}
 */

#endif /* __CLOCKUTILS_INIPARSER_INIPARSERPARAMETERS_H__ */

/**
 * @}
 */
