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
 * \addtogroup errors
 * @{
 */

#ifndef __CLOCKUTILS_CLOCKERRORS_H__
#define __CLOCKUTILS_CLOCKERRORS_H__

namespace clockUtils {

	enum class ClockError {
		SUCCESS,			//!< 0x0	method call succeeded
		// general
		NOT_READY,			//!< 0x1	not initialized yet
		TIMEOUT,			//!< 0x2	waiting time expired
		INVALID_ARGUMENT,	//!< 0x3	parameters are invalid
		INVALID_USAGE,		//!< 0x4	library is used in wrong way
		IN_PROGRESS,		//!< 0x5	current action already in progress
		WRONG_TYPE,			//!< 0x6	value couldn't be cast in expected type
		WRONG_SYNTAX,		//!< 0x7	syntax of parsed file is not as expected
		FILENOTFOUND,		//!< 0x8	file does not exist
		// sockets
		INVALID_IP,			//!< 0x9	ip address invalid
		INVALID_PORT,		//!< 0xA	port invalid
		ADDRESS_INUSE,		//!< 0xB	a socket is already bound to this address
		CONNECTION_FAILED,	//!< 0xC	connecting sockets failed
		NOT_CONNECTED,		//!< 0xD	using a method for connected sockets on unconnected socket
		// iniParser
		VALUE_NOTFOUND,		//!< 0xE	value not found
		//
		UNKNOWN,			//!< 0xF	an error occured, but the error isn't wrapped yet
		ERROR_COUNT
	};

} /* namespace clockUtils */

#endif /* __CLOCKUTILS_CLOCKERRORS_H__ */

/**
 * @}
 */
