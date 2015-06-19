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

#ifndef __CLOCKUTILS_CLOCKERRORS_H__
#define __CLOCKUTILS_CLOCKERRORS_H__

namespace clockUtils {

	enum class ClockError {
		SUCCESS,
		// general
		NOT_READY,
		TIMEOUT,
		INVALID_ARGUMENT,	// !< parameters are invalid
		INVALID_USAGE,		// !< library is used in wrong way
		IN_PROGRESS,
		WRONG_TYPE,			// !< value couldn't be cast in expected type
		WRONG_SYNTAX,		// !< syntax of parsed file is not as expected
		FILENOTFOUND,		// !< file does not exist
		// sockets
		INVALID_IP,
		INVALID_PORT,
		ADDRESS_INUSE,
		CONNECTION_FAILED,
		NOT_CONNECTED,
		// iniParser
		VALUE_NOTFOUND,		// !< value not found
		//
		UNKNOWN,
		ERROR_COUNT
	};

} /* namespace clockUtils */

#endif /* __CLOCKUTILS_CLOCKERRORS_H__ */

/**
 * @}
 */
