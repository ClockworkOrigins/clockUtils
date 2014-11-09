/**
 * \addtogroup compression
 * @{
 */

#pragma once

#include "clockUtils/SystemParameters.h"

// Dynamic library import/export macro
#ifndef CLOCK_COMPRESSION_API
	#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		#ifdef CLOCKUTILS_BUILD_SHARED
			#ifdef clock_compression_EXPORTS
				#define CLOCK_COMPRESSION_API __declspec(dllexport)
			#else
				#define CLOCK_COMPRESSION_API __declspec(dllimport)
			#endif
		#else
			#define CLOCK_COMPRESSION_API
		#endif
	#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		#define CLOCK_COMPRESSION_API
	#else
		#define CLOCK_COMPRESSION_API
	#endif
#endif

/**
 * @}
 */
