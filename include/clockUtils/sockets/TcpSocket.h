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
#include <sstream>
#include <string>
#include <vector>
#include <thread>

#include "clockUtils/errors.h"

#include "clockUtils/sockets/socketsParameters.h"
#include "clockUtils/sockets/Commons.h"

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
	 * \brief class for sockets using tcp
	 * \section sec_writing Writing to the socket
	 * Two different writing modes are available: \b stream and \b packet.<br>
	 * \b Stream is the normal TCP behaviour. Several packets are sent that form a continous stream.
	 * Several writes can be received with a single read and a single write can be split up in several reads.
	 * It is the users task to concatenate and split the stream if necessary.<br>
	 * \b Packet is an addition provided by this socket class. A single packet is received as a single packet.
	 * This allows to easily send messages, objects, etc. without worrying about the TCP internals.
	 * The behaviour is similar to UDP sockets but with the reliability of TCP.
	 * Internally, the packets are mapped to a stream and extracted and the other endpoint.<br>
	 * \attention Never mix stream and packet on the same socket and retrieve the data with the same method like it was sent.
	 * Otherwise the socket is likely to crash. However, e.g., sending a string packet and retrieving the packet as a vector is possible.
	 * \note All write methods return SUCCESS after successfully sending the data (or storing it in the buffer with async operations).
	 * However, no guarantees about the endpoint receiving the data is given.
	 * Even shortly after the remote endpoint disconnected ungratefully, writes will return SUCCESS
	 */
	class CLOCK_SOCKETS_API TcpSocket {
	public:
		/**
		 * \brief this function type is used as accept callback, so every accepted socket will reach this function on host
		 */
		typedef std::function<void(TcpSocket *, ClockError)> acceptCallback;

		/**
		 * \brief this function type is used receiving a packet using receiveCallback and is called for every packet
		 */
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
		 * \param[in] maxParallelConnections the amount of connections being able to be handled at once
		 * \param[in] acceptMultiple if set to false, only one connection is accepted, otherwise infinite
		 * \param[in] acb the callback to be called for every accepted connection. Will also be called when socket is closed during listen or an error occurs. If no error occurs, ClockError::SUCCESS is passed as second parameter
		 */
		ClockError listen(uint16_t listenPort, int maxParallelConnections, bool acceptMultiple, const acceptCallback acb);

		/**
		 * \brief creates a connection to the given pair of IP and port
		 * \param[in] remoteIP the ip of the connection listening as std::string
		 * \param[in] remotePort the port of the connection listening
		 * \param[in] timeout the time in milliseconds a connect request should last in maximum
		 */
		ClockError connectToIP(const std::string & remoteIP, uint16_t remotePort, unsigned int timeout) {
			IPv4 ip = convertIP(remoteIP);
			if (ip == NO_IP) {
				// not a valid hostname
				return ClockError::INVALID_IP;
			}
			return connect(ip, remotePort, timeout);
		}

		/**
		 * \brief creates a connection to the given pair of hostname and port
		 * \param[in] remoteHostname the hostname of the connection listening as std::string
		 * \param[in] remotePort the port of the connection listening
		 * \param[in] timeout the time in milliseconds a connect request should last in maximum
		 */
		ClockError connectToHostname(const std::string & remoteHostname, uint16_t remotePort, unsigned int timeout) {
			IPv4 ip = resolveHostname(remoteHostname);
			if (ip == NO_IP) {
				// not a valid hostname
				return ClockError::INVALID_IP;
			}
			return connect(ip, remotePort, timeout);
		}

		/**
		 * \brief creates a connection to the given pair of IP and port
		 * \param[in] remoteIP the ip of the connection listening as IPv4 integer
		 * \param[in] remotePort the port of the connection listening
		 * \param[in] timeout the time in milliseconds a connect request should last in maximum
		 */
		ClockError connect(const IPv4 remoteIP, uint16_t remotePort, unsigned int timeout);

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
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writePacket(const void * str, const size_t length);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writePacket(const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writePacket(const std::string & str) {
			return writePacket(str.c_str(), str.size());
		}

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writePacketAsync(const void * str, const size_t length);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writePacketAsync(const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writePacketAsync(const std::string & str) {
			return writePacketAsync(std::vector<uint8_t>(str.begin(), str.end()));
		}

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 * this functions blocks until a packet is received
		 */
		ClockError receivePacket(std::vector<uint8_t> & buffer);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 */
		ClockError receivePacket(std::string & buffer);

		/**
		 * \brief calls the callback for every packet that is received on this socket
		 * This function will return immediately
		 */
		ClockError receiveCallback(packetCallback pcb);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError write(const void * str, size_t length);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError write(const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError write(const std::string & str) {
			return write(str.c_str(), str.length());
		}

		/**
		 * \brief sends a message asynchron, doesn't work with receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writeAsync(const void * str, const size_t length);

		/**
		 * \brief sends a message asynchron, doesn't work with receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writeAsync(const std::vector<uint8_t> & vec);

		/**
		 * \brief sends a message asynchron, doesn't work with receivePacket
		 * <br>
		 * See \ref sec_writing
		 */
		ClockError writeAsync(const std::string & str) {
			return writeAsync(std::vector<uint8_t>(str.begin(), str.end()));
		}

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

		/**
		 * \brief sends parameter as a packet being receivable using operator>> or receivePacket
		 * T is an enum value
		 */
		template<typename T>
		typename std::enable_if<std::is_enum<T>::value, TcpSocket &>::type operator<<(const T & a) {
			std::stringstream ss;
			ss << uint32_t(a);
			writePacket(ss.str());
			return *this;
		}

		/**
		 * \brief sends parameter as a packet being receivable using operator>> or receivePacket
		 * T has to be streamable
		 */
		template<typename T>
		typename std::enable_if<!std::is_enum<T>::value, TcpSocket &>::type operator<<(const T & a) {
			std::stringstream ss;
			ss << a;
			writePacket(ss.str());
			return *this;
		}

		/**
		 * \brief receives a packet being sent using operator<< or writePacket(Async)
		 * T is an enum value
		 */
		template<typename T>
		typename std::enable_if<std::is_enum<T>::value, TcpSocket &>::type operator>>(T & a) {
			std::string buffer;
			receivePacket(buffer);
			std::stringstream ss(buffer);
			uint32_t e;
			ss >> e;
			a = T(e);
			return *this;
		}

		/**
		 * \brief receives a packet being sent using operator<< or writePacket(Async)
		 * T has to be streamable
		 */
		template<typename T>
		typename std::enable_if<!std::is_enum<T>::value, TcpSocket &>::type operator>>(T & a) {
			std::string buffer;
			receivePacket(buffer);
			std::stringstream ss(buffer);
			ss >> a;
			return *this;
		}

	private:
		/**
		 * \brief enumeration listing states a socket can have
		 */
		enum class SocketStatus {
			INACTIVE,	//<! socket was only created, but is neither listening nor connected
			LISTENING,	//<! socket is listening on connections
			CONNECTED	//<! socket is connected to another socket
		};

		/**
		 * \brief stores the local socket descriptor or INVALID_SOCKET if not active
		 */
		SOCKET _sock;

		/**
		 * \brief current socket status
		 */
		SocketStatus _status;

		/**
		 * \brief mutex guarding the queue for async packets
		 */
		std::mutex _writePacketAsyncLock;
		std::mutex _writeAsyncLock;

		/**
		 * \brief queue containing all packets to be sent in an extra worker thread
		 */
		std::queue<std::vector<uint8_t>> _writePacketAsyncQueue;
		std::queue<std::vector<uint8_t>> _writeAsyncQueue;

		/**
		 * \brief if a receivePacket gets more data than the packet contains, the rest is buffered in this variable
		 */
		std::vector<uint8_t> _buffer;

		bool _terminate;

		std::thread * _worker;
		std::thread * _listenThread;

		std::condition_variable _condVar;
		std::mutex _condMutex;

		std::thread * _callbackThread;

		/**
		 * \brief constructor being called during accept
		 * \param[in] fd file descriptor for the new socket
		 */
		TcpSocket(SOCKET fd);

		/**
		 * \brief reads platform specific error codes and returns a ClockError
		 */
		ClockError getLastError();

		/**
		 * \brief worker method for async write
		 */
		void work();

		void closeSocket();

		void listenFunc(SOCKET sock, bool acceptMultiple, const acceptCallback acb);

		TcpSocket(const TcpSocket &) = delete;
		TcpSocket & operator=(const TcpSocket &) = delete;
	};

	/**
	 * \brief specialization of stream operator for std::string to reduce overhead through converting to std::string using stringstream
	 */
	template<>
	CLOCK_SOCKETS_API TcpSocket & TcpSocket::operator<< <std::string>(const std::string & s);

	/**
	 * \brief specialization of stream operator for std::string to reduce overhead through converting to std::string using stringstream
	 */
	template<>
	CLOCK_SOCKETS_API TcpSocket & TcpSocket::operator>> <std::string>(std::string & s);

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_SOCKETS_TCPSOCKET_H__ */

/**
 * @}
 */
