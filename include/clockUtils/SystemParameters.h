/**
 * \addtogroup clockutils
 * @{
 */

#ifndef __CLOCKUTILS_SYSTEMPARAMETERS_H__
#define __CLOCKUTILS_SYSTEMPARAMETERS_H__

// Global platform specific defines
#define CLOCKUTILS_MPLATFORM_WIN32 1
#define CLOCKUTILS_MPLATFORM_LINUX 2
#define BOOST_ALL_DYN_LINK
#define WIN32_LEAN_AND_MEAN
// #define _WIN32_WINNT 0x0501
// Determine the current platform
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
	#define CLOCKUTILS_MPLATFORM CLOCKUTILS_MPLATFORM_WIN32
	#define WIN32_LEAN_AND_MEAN
#else
	#define CLOCKUTILS_MPLATFORM CLOCKUTILS_MPLATFORM_LINUX
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

/**
 * @}
 */

#endif /* __CLOCKUTILS_SYSTEMPARAMETERS_H__ */

/**
 * @}
 */
