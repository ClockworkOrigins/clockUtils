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

#include <errno.h>
#include <sstream>
#include <thread>

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <ws2tcpip.h>
#endif

namespace clockUtils {
namespace sockets {

	ClockError UdpSocket::bind(uint16_t port) {
		if (_sock != INVALID_SOCKET) {
			return ClockError::INVALID_USAGE;
		}

		struct sockaddr_in si;

		if ((_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
			ClockError error = getLastError();
			close();
			return error;
		}

		memset(reinterpret_cast<char *>(&si), 0, sizeof(si));
		si.sin_family = AF_INET;
		si.sin_port = htons(port);
		si.sin_addr.s_addr = htonl(INADDR_ANY);
		if (::bind(_sock, reinterpret_cast<struct sockaddr *>(&si), sizeof(si)) == -1) {
			ClockError error = getLastError();
			close();
			return error;
		}
		int stackSize = 1024 * 1024 * 1024; // 1 GB UDP stack
		int len = sizeof(int);
		setsockopt(_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&stackSize), len);
		setsockopt(_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&stackSize), len);

		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::writePacket(IPv4 ip, uint16_t port, const void * str, const size_t length) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		// | + size + str + |
		char * buf = new char[length + 6];

		buf[0] = '|';
		buf[1] = ((((length / 256) / 256) / 256) % 256);
		buf[2] = (((length / 256) / 256) % 256);
		buf[3] = ((length / 256) % 256);
		buf[4] = (length % 256);
		memcpy(reinterpret_cast<void *>(&buf[5]), str, length);
		buf[length + 5] = '|';

		ClockError error = write(ip, port, buf, length + 6);

		delete[] buf;
		return error;
	}

	ClockError UdpSocket::writePacket(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec) {
		return writePacket(ip, port, const_cast<const unsigned char *>(&vec[0]), vec.size());
	}

	ClockError UdpSocket::write(IPv4 ip, uint16_t port, const void * str, size_t length) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		struct sockaddr_in addr;

		memset(reinterpret_cast<char *>(&addr), 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = ip;

		for (size_t i = 0; i < length / MAX_PACKET_SIZE + 1; i++) {
			size_t sendLength = (i < length / MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : length - (i * MAX_PACKET_SIZE);
			if (sendto(_sock, &reinterpret_cast<const char *>(str)[i * MAX_PACKET_SIZE], int(sendLength), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
				ClockError error = getLastError();
				return error;
			}
		}
		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::writePacketAsync(IPv4 ip, uint16_t port, const void * str, const size_t length) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		std::vector<uint8_t> vec(length);
		for (size_t i = 0; i < length; i++) {
			vec[i] = reinterpret_cast<uint8_t *>(const_cast<void *>(str))[i];
		}
		_writePacketAsyncLock.lock();
		_writePacketAsyncQueue.push(std::make_tuple(vec, ip, port));
		_writePacketAsyncLock.unlock();
		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::writePacketAsync(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		_writePacketAsyncLock.lock();
		_writePacketAsyncQueue.push(std::make_tuple(vec, ip, port));
		_writePacketAsyncLock.unlock();
		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::writeAsync(IPv4 ip, uint16_t port, const void * str, const size_t length) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		std::vector<uint8_t> vec(length);
		for (size_t i = 0; i < length; i++) {
			vec[i] = reinterpret_cast<uint8_t *>(const_cast<void *>(str))[i];
		}
		_writeAsyncLock.lock();
		_writeAsyncQueue.push(std::make_tuple(vec, ip, port));
		_writeAsyncLock.unlock();
		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::writeAsync(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		_writeAsyncLock.lock();
		_writeAsyncQueue.push(std::make_tuple(vec, ip, port));
		_writeAsyncLock.unlock();

		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::receivePacket(std::vector<uint8_t> & buffer, std::string & ip, uint16_t & port) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		std::string s;

		ClockError error = receivePacket(s, ip, port);

		if (error == ClockError::SUCCESS) {
			buffer = std::vector<uint8_t>(s.begin(), s.end());
		}

		return error;
	}

	ClockError UdpSocket::receivePacket(std::string & buffer, std::string & ip, uint16_t & port) {
		if (_sock == INVALID_SOCKET) {
			return ClockError::NOT_READY;
		}
		std::vector<uint8_t> result;

		// TODO: (Daniel) check all buffers if one has a complete message

		while (true) {
			uint32_t length = 0;
			std::vector<uint8_t> s;
			ClockError error = ClockError::SUCCESS;

			error = read(s, ip, port);

			if (error != ClockError::SUCCESS) {
				return error;
			}

			if (s.empty()) {
				continue;
			}

			result = _buffer[std::make_pair(ip, port)];

			if (result.size() == 0) {
				if (s[0] == '|') {
					result = s;
				} else {
					return ClockError::UNKNOWN;
				}
			} else {
				result.insert(result.end(), s.begin(), s.end());
			}

			if (length == 0) {
				if (result.size() >= 5) {
					length = uint32_t(result[1] * 256 * 256 * 256 + result[2] * 256 * 256 + result[3] * 256 + result[4]);
				}
			}

			if (result.size() >= length + 6) {
				buffer = std::string(result.begin() + 5, result.begin() + 5 + int(length));

				if (result.size() > length + 6) {
					_buffer[std::make_pair(ip, port)] = std::vector<uint8_t>(result.begin() + int(length) + 6, result.end());
				} else {
					_buffer[std::make_pair(ip, port)].clear();
				}

				return ClockError::SUCCESS;
			} else {
				_buffer[std::make_pair(ip, port)] = std::vector<uint8_t>(result.begin(), result.end());
			}
		}

		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::receiveCallback(packetCallback pcb) {
		if (_callbackThread != nullptr) {
			_callbackThread->join();
			delete _callbackThread;
		}
		_callbackThread = new std::thread([pcb, this]() {
			while (_sock != INVALID_SOCKET) {
				std::vector<uint8_t> buffer;
				std::string ip;
				uint16_t port;
				ClockError err = receivePacket(buffer, ip, port);
				pcb(buffer, ip, port, err);
				if (err != ClockError::SUCCESS) {
					break;
				}
			}
		});
		return ClockError::SUCCESS;
	}

	void UdpSocket::work() {
		bool finish = false;
		// loop until finish is set. This ensures handling the pending writes
		do {
			{ // synchronize with destructor. Inside a scope to unlock _condMutex after waiting
				// aquire the condition mutex
				std::unique_lock<std::mutex> ul(_condMutex);
				// The normal case is waiting for new write request
				// Exceptions are:
				//	* _terminate  -> socket will be closed soon
				//	* size() != 0 -> new requests arrived
				// this check needs to be inside the lock to avoid a lost wake-up
				// if this 'if' or the 'else if' is true, the lock ensures that the notify is called *after* the wait call
				if (_terminate) {
					finish = true;
				} else if (_writePacketAsyncQueue.size() != 0 || _writeAsyncQueue.size() != 0) {
					// nothing
				} else {
					_condVar.wait(ul);
				}
			}
			_writePacketAsyncLock.lock();
			while (_writePacketAsyncQueue.size() > 0) {
				std::tuple<std::vector<uint8_t>, IPv4, uint16_t> tmp = std::move(_writePacketAsyncQueue.front());
				_writePacketAsyncLock.unlock();

				writePacket(std::get<AsyncQueueInfo::IP>(tmp), std::get<AsyncQueueInfo::Port>(tmp), const_cast<const unsigned char *>(&std::get<AsyncQueueInfo::Message>(tmp)[0]), std::get<AsyncQueueInfo::Message>(tmp).size());

				_writePacketAsyncLock.lock();
				_writePacketAsyncQueue.pop();
			}
			_writePacketAsyncLock.unlock();
			_writeAsyncLock.lock();
			while (_writeAsyncQueue.size() > 0) {
				std::tuple<std::vector<uint8_t>, IPv4, uint16_t> tmp = std::move(_writeAsyncQueue.front());
				_writeAsyncLock.unlock();

				write(std::get<AsyncQueueInfo::IP>(tmp), std::get<AsyncQueueInfo::Port>(tmp), const_cast<const unsigned char *>(&std::get<AsyncQueueInfo::Message>(tmp)[0]), std::get<AsyncQueueInfo::Message>(tmp).size());

				_writeAsyncLock.lock();
				_writeAsyncQueue.pop();
			}
			_writeAsyncLock.unlock();
		} while (!finish);
	}

} /* namespace sockets */
} /* namespace clockUtils */
