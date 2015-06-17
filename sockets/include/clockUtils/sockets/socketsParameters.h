/**
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can
 * redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

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
