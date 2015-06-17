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

#include "clockUtils/sockets/TcpSocket.h"

#include <chrono>
#include <thread>

#include "clockUtils/errors.h"

namespace clockUtils {
namespace sockets {

	ClockError TcpSocket::getLastError() {
		int error = WSAGetLastError();

		if (error == WSAEADDRINUSE) {
			return ClockError::ADDRESS_INUSE;
		} else if (error == WSA_INVALID_PARAMETER) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSA_INVALID_HANDLE) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSA_OPERATION_ABORTED) {
			return ClockError::NOT_READY;
		} else if (error == WSAEINTR) {
			return ClockError::NOT_READY;
		} else if (error == WSAEBADF) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEACCES) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEFAULT) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEINVAL) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEWOULDBLOCK) {
			return ClockError::IN_PROGRESS;
		} else if (error == WSAEINPROGRESS) {
			return ClockError::IN_PROGRESS;
		} else if (error == WSAEALREADY) {
			return ClockError::NOT_READY;
		} else if (error == WSAENOTSOCK) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEDESTADDRREQ) {
			return ClockError::INVALID_IP;
		} else if (error == WSAEADDRNOTAVAIL) {
			return ClockError::INVALID_PORT;
		} else if (error == WSAENETDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAENETUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAENETUNREACH) {
			return ClockError::NOT_READY;
		} else if (error == WSAECONNRESET) {
			return ClockError::NOT_READY;
		} else if (error == WSAEISCONN) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAENOTCONN) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAESHUTDOWN) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAETIMEDOUT) {
			return ClockError::TIMEOUT;
		} else if (error == WSAECONNREFUSED) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAEHOSTDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAEHOSTUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSASYSNOTREADY) {
			return ClockError::NOT_READY;
		} else if (error == WSAECONNABORTED) {
			return ClockError::NOT_CONNECTED;
		}

		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
