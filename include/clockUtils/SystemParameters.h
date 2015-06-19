/*
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * \addtogroup clockutils
 * @{
 */

#ifndef __CLOCKUTILS_SYSTEMPARAMETERS_H__
#define __CLOCKUTILS_SYSTEMPARAMETERS_H__

// Global platform specific defines
#define CLOCKUTILS_PLATFORM_WIN32 1
#define CLOCKUTILS_PLATFORM_LINUX 2
#define BOOST_ALL_DYN_LINK
#define WIN32_LEAN_AND_MEAN
// #define _WIN32_WINNT 0x0501
// Determine the current platform
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
	#define CLOCKUTILS_PLATFORM CLOCKUTILS_PLATFORM_WIN32
	#define WIN32_LEAN_AND_MEAN
#else
	#define CLOCKUTILS_PLATFORM CLOCKUTILS_PLATFORM_LINUX
#endif

// Debug/Release modes
#define CLOCKUTILS_DEBUG	1
#define CLOCKUTILS_RELEASE 2

#if defined(NDEBUG)
	#define CLOCKUTILS_MODE CLOCKUTILS_RELEASE
#else
	#define CLOCKUTILS_MODE CLOCKUTILS_DEBUG
#endif

#ifdef _MSC_VER
	#pragma warning(disable : 4251) // dll interface to be used by clients...
#endif

#endif /* __CLOCKUTILS_SYSTEMPARAMETERS_H__ */

/**
 * @}
 */
