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
		SUCCESS,			//!< method call succeeded
		// general
		NOT_READY,			//!< not initialized yet
		TIMEOUT,			//!< waiting time expired
		INVALID_ARGUMENT,	//!< parameters are invalid
		INVALID_USAGE,		//!< library is used in wrong way
		IN_PROGRESS,		//!< current action already in progress
		WRONG_TYPE,			//!< value couldn't be cast in expected type
		WRONG_SYNTAX,		//!< syntax of parsed file is not as expected
		FILENOTFOUND,		//!< file does not exist
		// sockets
		INVALID_IP,			//!< ip address invalid
		INVALID_PORT,		//!< port invalid
		ADDRESS_INUSE,		//!< a socket is already bound to this address
		CONNECTION_FAILED,	//!< connecting sockets failed
		NOT_CONNECTED,		//!< using a method for connected sockets on unconnected socket
		// iniParser
		VALUE_NOTFOUND,		//!< value not found
		//
		UNKNOWN,			//!< an error occured, but the error isn't wrapped yet
		ERROR_COUNT
	};

} /* namespace clockUtils */

#endif /* __CLOCKUTILS_CLOCKERRORS_H__ */

/**
 * @}
 */
