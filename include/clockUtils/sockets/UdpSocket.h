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

#ifndef __CLOCKUTILS_SOCKETS_UDPSOCKET_H__
#define __CLOCKUTILS_SOCKETS_UDPSOCKET_H__

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <thread>

#include "clockUtils/errors.h"

#include "clockUtils/sockets/socketsParameters.h"
#include "clockUtils/sockets/Commons.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <WinSock2.h>

	typedef int32_t socklen_t;

	// already defined by MSVC
	// #define INVALID_SOCKET -1
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	#include <arpa/inet.h>
	#include <cstring>
	#include <fcntl.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <unistd.h>

	typedef int SOCKET;
	#define INVALID_SOCKET -1
#endif

namespace std {
	class thread;
} /* namespace std */

namespace clockUtils {
	enum class ClockError;
namespace sockets {

	/**
	 * \brief class for sockets using udp
	 */
	class CLOCK_SOCKETS_API UdpSocket {
	public:
		/**
		 * \brief this function type is used receiving a packet using receiveCallback and is called for every packet
		 */
		typedef std::function<void(std::vector<uint8_t> packet, std::string ip, uint16_t port, ClockError err)> packetCallback;

		/**
		 * \brief maximum size one packet is allowed to have, otherwise it can't be sent
		 */
		const size_t MAX_PACKET_SIZE = 32 * 1024;

		/**
		 * \brief constructor
		 */
		UdpSocket();

		/**
		 * \brief destructor, closes socket if not happened before
		 */
		~UdpSocket();

		/**
		 * \brief binds this socket to the given port for message receiving
		 */
		ClockError bind(uint16_t port);

		/**
		 * \brief closes a connection if socket is connected
		 */
		void close();

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(IPv4 ip, uint16_t port, const void * str, const size_t length);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(IPv4 ip, uint16_t port, const std::string & str) {
			return writePacket(ip, port, str.c_str(), str.length());
		}

		/**
		 * \brief sends a packet asynchronous being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacketAsync(IPv4 ip, uint16_t port, const void * str, const size_t length);

		/**
		 * \brief sends a packet asynchronous being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacketAsync(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a packet asynchronous being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacketAsync(IPv4 ip, uint16_t port, const std::string & str) {
			return writePacketAsync(ip, port, std::vector<uint8_t>(str.begin(), str.end()));
		}

		/**
		 * \brief sends a packet, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(IPv4 ip, uint16_t port, const void * str, size_t length);

		/**
		 * \brief sends a packet, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec) {
			return write(ip, port, const_cast<const unsigned char *>(&vec[0]), vec.size());
		}

		/**
		 * \brief sends a packet, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(IPv4 ip, uint16_t port, const std::string & str) {
			return write(ip, port, str.c_str(), str.length());
		}

		/**
		 * \brief sends a message asynchronous, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writeAsync(IPv4 ip, uint16_t port, const void * str, size_t length);

		/**
		 * \brief sends a message asynchronous, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writeAsync(IPv4 ip, uint16_t port, const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a message asynchronous, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writeAsync(IPv4 ip, uint16_t port, const std::string & str) {
			return writeAsync(ip, port, std::vector<uint8_t>(str.begin(), str.end()));
		}
		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writePacketToIP(const std::string & ip, uint16_t port, T... data) {
			return writePacket(convertIP(ip), port, data...);
		}

		/**
		 * \brief sends a packet asynchronous being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writePacketToIPAsync(const std::string & ip, uint16_t port, T... data) {
			return writePacketAsync(convertIP(ip), port, data...);
		}

		/**
		 * \brief sends a packet, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writeToIP(const std::string & ip, uint16_t port, T... data) {
			return write(convertIP(ip), port, data...);
		}

		/**
		 * \brief sends a message asynchronous, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writeToIPAsync(const std::string & ip, uint16_t port, T... data) {
			return writeAsync(convertIP(ip), port, data...);
		}

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writePacketToHostname(const std::string & hostname, uint16_t port, T... data) {
			return writePacket(resolveHostname(hostname), port, data...);
		}

		/**
		 * \brief sends a packet asynchronous being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writePacketToHostnameAsync(const std::string & hostname, uint16_t port, T... data) {
			return writePacketAsync(resolveHostname(hostname), port, data...);
		}

		/**
		 * \brief sends a packet, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writeToHostname(const std::string & hostname, uint16_t port, T... data) {
			return write(resolveHostname(hostname), port, data...);
		}

		/**
		 * \brief sends a message asynchronous, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		template<typename... T>
		ClockError writeToHostnameAsync(const std::string & hostname, uint16_t port, T... data) {
			return writeAsync(resolveHostname(hostname), port, data...);
		}

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 * this functions blocks until a packet is received
		 * \param[out] buffer The data is stored in this buffer. Old data in this vector is deleted. Enough memory is allocated automatically.
		 * \param[out] ip The IP of the sender
		 * \param[out] port The port of the sender
		 */
		ClockError receivePacket(std::vector<uint8_t> & buffer, std::string & ip, uint16_t & port);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 * \param[out] buffer The data is stored in this buffer. Old data in this string is deleted. Enough memory is allocated automatically.
		 * \param[out] ip The IP of the sender
		 * \param[out] port The port of the sender
		 */
		ClockError receivePacket(std::string & buffer, std::string & ip, uint16_t & port);

		/**
		 * \brief calls the callback for every packet that is received on this socket
		 * This function will return immediately
		 */
		ClockError receiveCallback(packetCallback pcb);

		/**
		 * \brief receives data on the socket
		 */
		template<class Container>
		ClockError read(Container & buffer, std::string & ip, uint16_t & port) {
			if (_sock == INVALID_SOCKET) {
				return ClockError::NOT_READY;
			}

			buffer.resize(MAX_PACKET_SIZE + 4);
			int rc = -1;
			struct sockaddr_in remaddr;
			remaddr.sin_family = 0;
			remaddr.sin_port = 0;
			memset(&remaddr.sin_addr, 0, sizeof(remaddr.sin_addr));
			memset(remaddr.sin_zero, 0, 8);
			socklen_t addrlen = sizeof(remaddr);

			do {
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
				rc = recvfrom(_sock, &buffer[0], MAX_PACKET_SIZE, 0, (struct sockaddr *) &remaddr, &addrlen);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
				rc = recvfrom(_sock, reinterpret_cast<char *>(&buffer[0]), int(MAX_PACKET_SIZE), 0, (struct sockaddr *) &remaddr, &addrlen);
#endif

				port = static_cast<uint16_t>(ntohs(remaddr.sin_port));
				ip = inet_ntoa(remaddr.sin_addr);

				if (rc == -1) {
					ClockError error = getLastError();

					if (error == ClockError::IN_PROGRESS) {
						continue;
					}

					return error;
				} else if (rc == 0) {
					return ClockError::NOT_CONNECTED;
				}

				break;
			} while (true);

			buffer.resize(size_t(rc));

			return ClockError::SUCCESS;
		}

	private:
		/**
		 * \brief stores the local socket descriptor or INVALID_SOCKET if not active
		 */
		SOCKET _sock;

		/**
		 * \brief if a receivePacket gets more data than the packet contains, the rest is buffered in this variable
		 */
		std::map<std::pair<std::string, uint16_t>, std::vector<uint8_t>> _buffer;

		/**
		 * \brief thread for polling messages
		 */
		std::thread * _callbackThread;

		/**
		 * \brief mutex guarding the queue for async packets
		 */
		std::mutex _writePacketAsyncLock;
		std::mutex _writeAsyncLock;

		/**
		 * \brief queue containing all packets to be sent in an extra worker thread
		 */
		enum AsyncQueueInfo {
			Message,
			IP,
			Port
		};
		std::queue<std::tuple<std::vector<uint8_t>, IPv4, uint16_t>> _writePacketAsyncQueue;
		std::queue<std::tuple<std::vector<uint8_t>, IPv4, uint16_t>> _writeAsyncQueue;

		std::condition_variable _condVar;
		std::mutex _condMutex;

		std::thread * _worker;

		bool _terminate;

		/**
		 * \brief reads platform specific error codes and returns a ClockError
		 */
		ClockError getLastError();

		void work();

		UdpSocket(const UdpSocket &) = delete;
		UdpSocket & operator=(const UdpSocket &) = delete;
	};

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_SOCKETS_UDPSOCKET_H__ */

/**
 * @}
 */
