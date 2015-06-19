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

#ifndef __CLOCKUTILS_SOCKETS_TCPSOCKET_H__
#define __CLOCKUTILS_SOCKETS_TCPSOCKET_H__

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "clockUtils/sockets/socketsParameters.h"
#include "clockUtils/errors.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <WinSock2.h>

	typedef int32_t socklen_t;
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	#include <arpa/inet.h>
	#include <cstring>
	#include <fcntl.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

namespace std {
	class thread;
} /* namespace std */
namespace clockUtils {
	enum class ClockError;
namespace sockets {

	class CLOCK_SOCKETS_API TcpSocket {
	public:
		typedef std::function<void(TcpSocket *)> acceptCallback;

		typedef std::function<void(std::vector<uint8_t> packet, TcpSocket * socket, ClockError err)> packetCallback;

		/**
		 * \brief constructor
		 */
		TcpSocket();

		/**
		 * \brief destructor, closes socket if not happened before
		 */
		~TcpSocket();

		/**
		 * \brief creates listen socket listening on the given port and calling callback for every accepted connection
		 * \param[in] listenPort the port the socket is listening on
		 * \param[in] maxParallelConnections the amount of connections being able to be handle at once
		 * \param[in] acceptMultiple if set to false, only one connection is accepted, otherwise infinite
		 * \param[in] acb the callback to be called for every accepted connection
		 */
		ClockError listen(uint16_t listenPort, int maxParallelConnections, bool acceptMultiple, const acceptCallback acb);

		/**
		 * \brief creates a connection to the given pair of IP and port
		 * \param[in] remoteIP the ip of the connection listening
		 * \param[in] remotePort the port of the connection listening
		 * \param[in] timeout the time in milliseconds a connect request should last in maximum
		 */
		ClockError connect(const std::string & remoteIP, uint16_t remotePort, unsigned int timeout);

		/**
		 * \brief closes a connection if socket is connected
		 */
		void close();

		/**
		 * \brief returns the ip of the socket this socket is connected with
		 */
		std::string getRemoteIP() const;

		/**
		 * \brief returns the port of the socket this socket is connected with
		 */
		uint16_t getRemotePort() const;

		/**
		 * \brief returns all local IP's
		 */
		static std::vector<std::pair<std::string, std::string>> enumerateLocalIPs();

		/**
		 * \brief returns the local IP of this socket
		 */
		std::string getLocalIP() const;

		/**
		 * \brief returns the public IP of this socket shown in network
		 */
		std::string getPublicIP() const;

		/**
		 * \brief returns the port this socket uses for connection
		 */
		uint16_t getLocalPort() const;

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(const void * str, const uint32_t length);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(const std::vector<uint8_t> & str);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacketAsync(const std::vector<uint8_t> & str);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 * this functions blocks until a packet is received
		 */
		ClockError receivePacket(std::vector<uint8_t> & buffer);

		/**
		 * \brief calls the callback for every packet that is received on this socket
		 * This function will return immediately
		 */
		ClockError receiveCallback(packetCallback pcb);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 */
		ClockError receivePacket(std::string & buffer);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(const void * str, uint32_t length);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(const std::vector<uint8_t> & str);

		/**
		 * \brief receives data on the socket
		 */
		template<class Container>
		ClockError read(Container & buffer) {
			if (_status != SocketStatus::CONNECTED) {
				return ClockError::NOT_READY;
			}

			buffer.resize(260);
			int rc = -1;

			do {
	#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
				rc = recv(_sock, &buffer[0], 256, 0);
	#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
				rc = recv(_sock, reinterpret_cast<char *>(&buffer[0]), 256, 0);
	#endif

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

		/* void operator<<(int a);

		void operator>>(int & a); */

	private:
		/**
		 * \brief enumeration listing states a socket can have
		 */
		enum class SocketStatus {
			INACTIVE,	// ! socket was only created, but is neither listening nor connected
			LISTENING,	// ! socket is listening on connections
			CONNECTED	// ! socket is connected to another socket
		};

		/**
		 * \brief constructor being called during accept
		 * \param[in] fd file descriptor for the new socket
		 */
		TcpSocket(int fd);

		/**
		 * \brief reads platform specific error codes and returns a ClockError
		 */
		ClockError getLastError();

		/**
		 * \brief stores the local socket descriptor or -1 if not active
		 */
		int _sock;

		/**
		 * \brief current socket status
		 */
		SocketStatus _status;

		/**
		 * \brief mutex guarding the queue for async packets
		 */
		std::mutex _todoLock;

		/**
		 * \brief queue containing all packets to be sent in an extra worker thread
		 */
		std::queue<std::vector<uint8_t>> _todo;

		/**
		 * \brief if a receivePacket gets more data than the packet contains, the rest is buffered in this variable
		 */
		std::vector<uint8_t> _buffer;

		bool _terminate;

		std::thread * _worker;
		std::thread * _listenThread;

		std::condition_variable _objCondExecutable;
		std::mutex _objCondMut;
		std::unique_lock<std::mutex> _objCondUniqLock;

		std::thread * _callbackThread;

		TcpSocket(const TcpSocket &) = delete;
		TcpSocket & operator=(const TcpSocket &) = delete;
	};

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_SOCKETS_TCPSOCKET_H__ */

/**
 * @}
 */
