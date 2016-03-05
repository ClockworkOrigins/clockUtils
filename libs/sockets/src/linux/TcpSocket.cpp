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

#include "clockUtils/sockets/TcpSocket.h"

#ifndef _GLIBCXX_USE_NANOSLEEP
	#define _GLIBCXX_USE_NANOSLEEP // needed for sleep_for, see http://stackoverflow.com/questions/4438084/stdthis-threadsleep-for-and-gcc, but is fixed in newer version of gcc (>4.8 i think)
#endif

#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "clockUtils/errors.h"

#include <iostream>

namespace clockUtils {
namespace sockets {

	ClockError TcpSocket::getLastError() {
		if (errno == EADDRINUSE) {
			return ClockError::ADDRESS_INUSE;
		} else if (errno == EINTR) {
			return ClockError::NOT_READY;
		} else if (errno == EBADF) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EACCES) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EFAULT) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EINVAL) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EWOULDBLOCK) {
			return ClockError::IN_PROGRESS;
		} else if (errno == EINPROGRESS) {
			return ClockError::IN_PROGRESS;
		} else if (errno == EALREADY) {
			return ClockError::NOT_READY;
		} else if (errno == ENOTSOCK) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EDESTADDRREQ) {
			return ClockError::INVALID_IP;
		} else if (errno == EADDRNOTAVAIL) {
			return ClockError::INVALID_PORT;
		} else if (errno == ENETDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == ENETUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == ENETUNREACH) {
			return ClockError::NOT_READY;
		} else if (errno == ECONNRESET) {
			return ClockError::NOT_READY;
		} else if (errno == EISCONN) {
			return ClockError::INVALID_USAGE;
		} else if (errno == ENOTCONN) {
			return ClockError::INVALID_USAGE;
		} else if (errno == ESHUTDOWN) {
			return ClockError::INVALID_USAGE;
		} else if (errno == ETIMEDOUT) {
			return ClockError::TIMEOUT;
		} else if (errno == ECONNREFUSED) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == EHOSTDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == EHOSTUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == ECONNABORTED) {
			return ClockError::NOT_CONNECTED;
		} else if (errno == EPIPE) {
			return ClockError::NOT_CONNECTED;
		}
std::cerr << "Unknown error: " << errno << std::endl;
		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
