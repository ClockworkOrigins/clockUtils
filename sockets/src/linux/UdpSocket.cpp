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

#include "clockUtils/sockets/UdpSocket.h"

#ifndef _GLIBCXX_USE_NANOSLEEP
	#define _GLIBCXX_USE_NANOSLEEP // needed for sleep_for, see http://stackoverflow.com/questions/4438084/stdthis-threadsleep-for-and-gcc, but is fixed in newer version of gcc (>4.8 i think)
#endif

#include <chrono>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "clockUtils/errors.h"

namespace clockUtils {
namespace sockets {

	UdpSocket::UdpSocket() : _sock(-1), _buffer(), _callbackThread(nullptr), _writePacketAsyncLock(), _writeAsyncLock(), _writePacketAsyncQueue(), _writeAsyncQueue(), _objCondExecutable(), _objCondMut(), _objCondUniqLock(_objCondMut), _worker(nullptr), _terminate(false) {
		_worker = new std::thread([this]() {
			while (!_terminate) {
				_writePacketAsyncLock.lock();
				while (_writePacketAsyncQueue.size() > 0) {
					std::tuple<std::vector<uint8_t>, std::string, uint16_t> tmp = std::move(_writePacketAsyncQueue.front());
					_writePacketAsyncLock.unlock();

					writePacket(std::get<AsyncQueueInfo::IP>(tmp), std::get<AsyncQueueInfo::Port>(tmp), const_cast<const unsigned char *>(&std::get<AsyncQueueInfo::Message>(tmp)[0]), std::get<AsyncQueueInfo::Message>(tmp).size());

					_writePacketAsyncLock.lock();
					_writePacketAsyncQueue.pop();
				}
				_writePacketAsyncLock.unlock();
				_writeAsyncLock.lock();
				while (_writeAsyncQueue.size() > 0) {
					std::tuple<std::vector<uint8_t>, std::string, uint16_t> tmp = std::move(_writeAsyncQueue.front());
					_writeAsyncLock.unlock();

					write(std::get<AsyncQueueInfo::IP>(tmp), std::get<AsyncQueueInfo::Port>(tmp), const_cast<const unsigned char *>(&std::get<AsyncQueueInfo::Message>(tmp)[0]), std::get<AsyncQueueInfo::Message>(tmp).size());

					_writeAsyncLock.lock();
					_writeAsyncQueue.pop();
				}
				_writeAsyncLock.unlock();
				_objCondExecutable.wait(_objCondUniqLock);
			}
		});
	}

	UdpSocket::~UdpSocket() {
		_terminate = true;
		_objCondExecutable.notify_all();
		if (_worker->joinable()) {
			_worker->join();
		}
		delete _worker;
		close();
	}

	void UdpSocket::close() {
		if (_sock != -1) {
			::shutdown(_sock, SHUT_RDWR); // FIXME: only do this if connected?, check for errorcode than
			::close(_sock); // FIXME: only do this if connected?, check for errorcode than
			_sock = -1;
		}
		if (_callbackThread != nullptr) {
			_callbackThread->join();
			delete _callbackThread;
			_callbackThread = nullptr;
		}
	}

	ClockError UdpSocket::getLastError() {
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

		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
