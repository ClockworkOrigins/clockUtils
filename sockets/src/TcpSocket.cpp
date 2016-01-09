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

#include <errno.h>
#include <thread>

namespace clockUtils {
namespace sockets {

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	class WSAHelper {
	public:
		WSAHelper() {
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 2), &wsa);
		}

		~WSAHelper() {
			WSACleanup();
		}
	};
#endif

	TcpSocket::TcpSocket() : _sock(-1), _status(SocketStatus::INACTIVE), _writePacketAsyncLock(), _writeAsyncLock(), _writePacketAsyncQueue(), _writeAsyncQueue(), _buffer(), _terminate(false), _worker(nullptr), _listenThread(nullptr), _condVar(), _condMutex(), _callbackThread(nullptr) {
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		static WSAHelper wsa;
#endif
		_worker = new std::thread(std::bind(&TcpSocket::work, this));
	}

	TcpSocket::TcpSocket(SOCKET fd) : TcpSocket() {
		_sock = fd;
		_status = SocketStatus::CONNECTED;
	}

	TcpSocket::~TcpSocket() {
		_terminate = true;
		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		if (_worker->joinable()) {
			_worker->join();
		}
		delete _worker;
		close();
		if (_listenThread) {
			if (_listenThread->joinable()) {
				_listenThread->join();
			}
			delete _listenThread;
		}
	}

	ClockError TcpSocket::listen(uint16_t listenPort, int maxParallelConnections, bool acceptMultiple, const acceptCallback acb) {
		if (listenPort == 0) {
			return ClockError::INVALID_PORT;
		}
		if (_status != SocketStatus::INACTIVE) {
			return ClockError::INVALID_USAGE;
		}
		if (maxParallelConnections < 0) {
			return ClockError::INVALID_ARGUMENT;
		}
		_sock = socket(PF_INET, SOCK_STREAM, 0);
		if (-1 == _sock) {
			return ClockError::CONNECTION_FAILED;
		}
		if (_listenThread) {
			if (_listenThread->joinable()) {
				_listenThread->join();
			}
			delete _listenThread;
			_listenThread = nullptr;
		}

		// set reusable
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		char flag = 1;
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		int flag = 1;
#endif
		setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

		struct sockaddr_in name = { AF_INET, htons(listenPort), INADDR_ANY, {0} };
		errno = 0;
		if (-1 == bind(_sock, reinterpret_cast<struct sockaddr *>(&name), sizeof(name))) {
			ClockError error = getLastError();
			closeSocket();
			close();
			return error;
		}

		errno = 0;
		if (-1 == ::listen(_sock, maxParallelConnections)) {
			ClockError error = getLastError();
			closeSocket();
			close();
			return error;
		}

		_listenThread = new std::thread([acceptMultiple, acb, this]()
			{
				do {
					errno = 0;
					SOCKET clientSock = ::accept(_sock, nullptr, nullptr);
					if (clientSock == -1 || _terminate) {
						close();
						closeSocket();
						return;
					}
					std::thread thrd2(std::bind(acb, new TcpSocket(clientSock)));
					thrd2.detach();
				} while (acceptMultiple && !_terminate);
				close();
			});

		_status = SocketStatus::LISTENING;

		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::connect(const std::string & remoteIP, uint16_t remotePort, unsigned int timeout) {
		if (_status != SocketStatus::INACTIVE) {
			return ClockError::INVALID_USAGE;
		}

		if (remotePort == 0) {
			return ClockError::INVALID_PORT;
		}

		if (remoteIP.length() < 8) {
			return ClockError::INVALID_IP;
		}

		errno = 0;
		_sock = socket(PF_INET, SOCK_STREAM, 0);

		if (_sock == -1) {
			return getLastError();
		}

		sockaddr_in addr;

		memset(&addr, 0, sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(remotePort);
		addr.sin_addr.s_addr = inet_addr(remoteIP.c_str());

		if (addr.sin_addr.s_addr == INADDR_NONE || addr.sin_addr.s_addr == INADDR_ANY) {
			close();
			closeSocket();
			return ClockError::INVALID_IP;
		}

		// set socket non-blockable
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		u_long iMode = 1;
		ioctlsocket(_sock, FIONBIO, &iMode);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		long arg = fcntl(_sock, F_GETFL, NULL);
		arg |= O_NONBLOCK;
		fcntl(_sock, F_SETFL, arg);
#endif

		// connect
		errno = 0;
		if (-1 == ::connect(_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr))) {
			ClockError error = getLastError();
			if (error == ClockError::IN_PROGRESS) {
				// connect still in progress. wait for completion with timeout
				struct timeval tv;
				fd_set myset;
				tv.tv_sec = time_t(timeout / 1000);
				tv.tv_usec = (timeout % 1000) * 1000;
				FD_ZERO(&myset);
				FD_SET(_sock, &myset);
				if (select(int(_sock + 1), NULL, &myset, NULL, &tv) > 0) {
					socklen_t lon = sizeof(int);
					int valopt;

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
					getsockopt(_sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&valopt), &lon);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
					getsockopt(_sock, SOL_SOCKET, SO_ERROR, static_cast<void *>(&valopt), &lon);
#endif
					if (valopt) {
						closeSocket();
						close();
						return ClockError::CONNECTION_FAILED;
					}
				} else {
					closeSocket();
					close();
					return ClockError::TIMEOUT;
				}
			} else {
				closeSocket();
				close();
				return error;
			}
		}

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		iMode = 0;
		ioctlsocket(_sock, FIONBIO, &iMode);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		arg &= (~O_NONBLOCK);
		fcntl(_sock, F_SETFL, arg);
#endif

		_status = SocketStatus::CONNECTED;

		return ClockError::SUCCESS;
	}

	std::string TcpSocket::getRemoteIP() const {
		if (_status == SocketStatus::INACTIVE) {
			return "";
		}
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getpeername(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return inet_ntoa(localAddress.sin_addr);
	}

	uint16_t TcpSocket::getRemotePort() const {
		if (_status == SocketStatus::INACTIVE) {
			return 0;
		}
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getpeername(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return static_cast<uint16_t>(ntohs(localAddress.sin_port));
	}

	std::vector<std::pair<std::string, std::string>> TcpSocket::enumerateLocalIPs() {
		char buffer[256];
		gethostname(buffer, 256);
		hostent * localHost = gethostbyname(buffer);

		std::vector<std::pair<std::string, std::string>> result;

		for (int i = 0; *(localHost->h_addr_list + i) != nullptr; i++) {
			char * localIP = inet_ntoa(*reinterpret_cast<struct in_addr *>(*(localHost->h_addr_list + i)));

			result.push_back(std::make_pair(std::string(localHost->h_name), std::string(localIP)));
		}

		return result;
	}

	std::string TcpSocket::getLocalIP() const {
		char buffer[256];
		gethostname(buffer, 256);
		hostent * localHost = gethostbyname(buffer);
		char * localIP = inet_ntoa(*reinterpret_cast<struct in_addr *>(*localHost->h_addr_list));

		std::string ip(localIP);

		return ip;
	}

	std::string TcpSocket::getPublicIP() const {
		if (_status == SocketStatus::INACTIVE) {
			return "";
		}
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getsockname(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return inet_ntoa(localAddress.sin_addr);
	}

	uint16_t TcpSocket::getLocalPort() const {
		if (_status == SocketStatus::INACTIVE) {
			return 0;
		}
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getsockname(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return static_cast<uint16_t>(ntohs(localAddress.sin_port));
	}

	ClockError TcpSocket::writePacket(const void * str, const size_t length) {
		if (_status != SocketStatus::CONNECTED) {
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

		ClockError error = write(buf, length + 6);

		delete[] buf;
		return error;
	}

	ClockError TcpSocket::writePacket(const std::vector<uint8_t> & vec) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}
		return writePacket(const_cast<const unsigned char *>(&vec[0]), vec.size());
	}

	ClockError TcpSocket::writePacketAsync(const void * str, const size_t length) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

		std::vector<uint8_t> vec(length);
		for (size_t i = 0; i < length; i++) {
			vec[i] = reinterpret_cast<uint8_t *>(const_cast<void *>(str))[i];
		}
		_writePacketAsyncLock.lock();
		_writePacketAsyncQueue.push(vec);
		_writePacketAsyncLock.unlock();
		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::writePacketAsync(const std::vector<uint8_t> & vec) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

		_writePacketAsyncLock.lock();
		_writePacketAsyncQueue.push(vec);
		_writePacketAsyncLock.unlock();
		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::writeAsync(const void * str, const size_t length) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

		std::vector<uint8_t> vec(length);
		for (size_t i = 0; i < length; i++) {
			vec[i] = reinterpret_cast<uint8_t *>(const_cast<void *>(str))[i];
		}
		_writeAsyncLock.lock();
		_writeAsyncQueue.push(vec);
		_writeAsyncLock.unlock();

		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::writeAsync(const std::vector<uint8_t> & vec) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

		_writeAsyncLock.lock();
		_writeAsyncQueue.push(vec);
		_writeAsyncLock.unlock();

		{
			std::unique_lock<std::mutex> ul(_condMutex);
			_condVar.notify_all();
		}
		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::receivePacket(std::vector<uint8_t> & buffer) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

		std::string s;

		ClockError error = receivePacket(s);

		if (error == ClockError::SUCCESS) {
			buffer = std::vector<uint8_t>(s.begin(), s.end());
		}

		return error;
	}

	ClockError TcpSocket::receivePacket(std::string & buffer) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

		std::vector<uint8_t> result(_buffer);

		bool skipFirstRead = !result.empty();

		uint32_t length = 0;

		while (true) {
			std::vector<uint8_t> s;
			ClockError error = ClockError::SUCCESS;

			if (!skipFirstRead) {
				error = read(s);
			}

			if (error != ClockError::SUCCESS) {
				return error;
			}

			if (!skipFirstRead && s.empty()) {
				continue;
			}
			skipFirstRead = false;
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
					_buffer = std::vector<uint8_t>(result.begin() + int(length) + 6, result.end());
				} else {
					_buffer.clear();
				}

				return ClockError::SUCCESS;
			}
		}

		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::write(const void * str, size_t length) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
		int rc = send(_sock, reinterpret_cast<const char *>(str), length, MSG_NOSIGNAL);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		int rc = send(_sock, reinterpret_cast<const char *>(str), int(length), 0);
#endif

		if (rc == -1) {
			return getLastError();
		} else if (rc == 0) {
			return ClockError::NOT_CONNECTED;
		}

		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::write(const std::vector<uint8_t> & str) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}
		return write(const_cast<const unsigned char *>(&str[0]), str.size());
	}

	ClockError TcpSocket::receiveCallback(packetCallback pcb) {
		if (_status != SocketStatus::CONNECTED) {
			return ClockError::NOT_READY;
		}
		if (_callbackThread != nullptr) {
			_callbackThread->join();
			delete _callbackThread;
		}
		_callbackThread = new std::thread([pcb, this]()
			{
				while (_sock != -1) {
					std::vector<uint8_t> buffer;
					ClockError err = receivePacket(buffer);
					pcb(buffer, this, err);
					if (err != ClockError::SUCCESS) {
						break;
					}
				}
			});
		return ClockError::SUCCESS;
	}

	void TcpSocket::close() {
		if (_status != SocketStatus::INACTIVE) {
			// needed to stop pending accept operations
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
			::shutdown(_sock, SHUT_RDWR); // can fail, but doesn't matter. Was e.g. not connected before
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
			shutdown(_sock, SD_BOTH);
#endif
			_status = SocketStatus::INACTIVE;
		}
		closeSocket();
		try {
			if (_callbackThread != nullptr) {
				if (_callbackThread->joinable()) {
					_callbackThread->join();
				}
				delete _callbackThread;
				_callbackThread = nullptr;
			}
		} catch (std::system_error &) {
			// this can only be a deadlock, so do nothing here and delete thread in destructor
		}
	}

	template<>
	TcpSocket & TcpSocket::operator<< <std::string>(const std::string & s) {
		writePacket(s);
		return *this;
	}
	
	template<>
	TcpSocket & TcpSocket::operator>> <std::string>(std::string & s) {
		receivePacket(s);
		return *this;
	}
	
	void TcpSocket::work() {
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
				std::vector<uint8_t> tmp = std::move(_writePacketAsyncQueue.front());
				_writePacketAsyncQueue.pop();
				_writePacketAsyncLock.unlock();

				writePacket(const_cast<const unsigned char *>(&tmp[0]), tmp.size());

				_writePacketAsyncLock.lock();
			}
			_writePacketAsyncLock.unlock();
			_writeAsyncLock.lock();
			while (_writeAsyncQueue.size() > 0) {
				std::vector<uint8_t> tmp = std::move(_writeAsyncQueue.front());
				_writeAsyncQueue.pop();
				_writeAsyncLock.unlock();

				write(const_cast<const unsigned char *>(&tmp[0]), tmp.size());

				_writeAsyncLock.lock();
			}
			_writeAsyncLock.unlock();
		} while(!finish);
	}

	void TcpSocket::closeSocket() {
		if (_sock != -1) {
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
			::close(_sock); // can fail, but doesn't matter. Was e.g. not connected before
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
			closesocket(_sock);
#endif
			_sock = -1;
		}
	}

} /* namespace sockets */
} /* namespace clockUtils */
