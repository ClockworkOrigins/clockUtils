/**
 * \addtogroup sockets
 * @{
 */

#ifndef __CLOCKUTILS_SOCKETS_SOCKETSPARAMETERS_H__
#define __CLOCKUTILS_SOCKETS_SOCKETSPARAMETERS_H__

#include "clockUtils/SystemParameters.h"

// Dynamic library import/export macro
#ifndef CLOCK_SOCKETS_API
	#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		#ifdef CLOCKUTILS_BUILD_SHARED
			#ifdef clock_sockets_EXPORTS
				#define CLOCK_SOCKETS_API __declspec(dllexport)
			#else
				#define CLOCK_SOCKETS_API __declspec(dllimport)
			#endif
		#else
			#define CLOCK_SOCKETS_API
		#endif
	#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		#define CLOCK_SOCKETS_API
	#else
		#define CLOCK_SOCKETS_API
	#endif
#endif

/**
 * @}
 */

#endif /* __CLOCKUTILS_SOCKETS_SOCKETSPARAMETERS_H__ */

/**
 * @}
 */
