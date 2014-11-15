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
}
namespace clockUtils {
	enum class ClockError;

namespace sockets {

	class CLOCK_SOCKETS_API UdpSocket {
	public:
		typedef std::function<void(std::vector<uint8_t> packet, UdpSocket * socket, ClockError err)> packetCallback;

		const int MAX_PACKET_SIZE = 32 * 1024;

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
		 *
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(const std::string & ip, uint16_t port, const void * str, const uint32_t length);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 *
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(const std::string & ip, uint16_t port, const std::vector<uint8_t> & str);

		/**
		 * \brief TODO
		 */
		ClockError write(const std::string & ip, uint16_t port, const void * str, uint32_t length);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 * this functions blocks until a packet is received
		 */
		ClockError receivePacket(std::vector<uint8_t> & buffer, std::string & ip, uint16_t & port);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 */
		ClockError receivePacket(std::string & buffer, std::string & ip, uint16_t & port);

		/**
		 * \brief receives data on the socket
		 */
		template<class Container>
		ClockError read(Container & buffer, std::string & ip, uint16_t & port) {
			if (_sock == -1) {
				return ClockError::NOT_READY;
			}

			buffer.resize(MAX_PACKET_SIZE + 4);
			int rc = -1;
			struct sockaddr_in remaddr;
			socklen_t addrlen = sizeof(remaddr);

			do {
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
				rc = recvfrom(_sock, &buffer[0], MAX_PACKET_SIZE, 0, (struct sockaddr *) &remaddr, &addrlen);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
				rc = recvfrom(_sock, reinterpret_cast<char *>(&buffer[0]), MAX_PACKET_SIZE, 0, (struct sockaddr *) &remaddr, &addrlen);
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
		 * \brief reads platform specific error codes and returns a ClockError
		 */
		ClockError getLastError();

		/**
		 * \brief stores the lokal socket descriptor or -1 if not active
		 */
		int _sock;

		std::mutex _todoLock;
		std::queue<std::vector<uint8_t>> _todo;

		/**
		 * \brief if a receivePacket gets more data than the packet contains, the rest is buffered in this variable
		 */
		std::map<std::pair<std::string, uint16_t>, std::vector<uint8_t>> _buffer;

		bool _terminate;

		std::thread * _worker;

		std::condition_variable _objCondExecutable;
		std::mutex _objCondMut;
		std::unique_lock<std::mutex> _objCondUniqLock;

		UdpSocket(const UdpSocket &) = delete;
		UdpSocket & operator=(const UdpSocket &) = delete;
	};

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_SOCKETS_UDPSOCKET_H__ */
