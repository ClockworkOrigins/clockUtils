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
 * \addtogroup sockets
 * @{
 */

#ifndef __CLOCKUTILS_SOCKETS_COMMONS_H__
#define __CLOCKUTILS_SOCKETS_COMMONS_H__

#include <cstdint>
#include <string>

#include "clockUtils/sockets/socketsParameters.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <WinSock2.h>
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_UNIX
	#include <netinet/in.h>
#endif

namespace clockUtils {
namespace sockets {

	typedef uint32_t IPv4;

	const IPv4 NO_IP = INADDR_NONE;

	/**
	 * \brief returns the IP for a given hostname (URL)
	 */
	CLOCK_SOCKETS_API IPv4 resolveHostname(const std::string & url);

	/**
	 * \brief converts a std::string formatted IP to IPv4 integer
	 */
	CLOCK_SOCKETS_API IPv4 convertIP(const std::string & ip);

	/**
	 * \brief converts an IPv4 formatted IP to std::string
	 */
	CLOCK_SOCKETS_API std::string convertIP(const IPv4 & ip);

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_SOCKETS_COMMONS_H__ */
