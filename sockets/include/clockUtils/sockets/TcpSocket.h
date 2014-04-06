#ifndef __SOCKETS_TCPSOCKET_H__
#define __SOCKETS_TCPSOCKET_H__

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "clockUtils/errors.h"
#include "clockUtils/sockets/socketsParameters.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <mutex>
#endif

namespace clockUtils {
namespace sockets {

	class CLOCK_SOCKETS_API TcpSocket {
	public:
		typedef std::function<void(TcpSocket *)> acceptCallback;

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
		 *
		 * \param[in] listenPort the port the socket is listening on
		 * \param[in] maxParallelConnections the amount of connections being able to be handle at once
		 * \param[in] acceptMultiple if set to false, only one connection is accepted, otherwise infinite
		 * \param[in] acb the callback to be called for every accepted connection
		 */
		ClockError listen(uint16_t listenPort, int maxParallelConnections, bool acceptMultiple, const acceptCallback acb);
	
		/**
		 * \brief creates a connection to the given pair of IP and port
		 *
		 * \param[in] remoteIP the ip of the connection listening
		 * \param[in] remotePort the port of the connection listening
		 * \param[in] timeout the time in milliseconds a connect request should least in maximum
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
		 *
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(const void * str, const size_t length);

		/**
		 * \brief sends a packet being able to be completely received in one call of receivePacket
		 *
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError writePacket(const std::vector<uint8_t> & str);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 */
		ClockError receivePacket(std::vector<uint8_t> & buffer);

		/**
		 * \brief receives a packet sent with writePacket, doesn't work with write
		 */
		ClockError receivePacket(std::string & buffer);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 *
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(const void * str, size_t length);

		/**
		 * \brief sends a message, doesn't work with receivePacket
		 *
		 * \return if packet was sent, the method returns ClockError::SUCCESS, otherwise one of the other error codes. Can also return SUCCESS, if the socket was closed by peer and it wasn't detected yet
		 */
		ClockError write(const std::vector<uint8_t> & str);

		/**
		 * \brief receives data on the socket
		 */
		ClockError read(std::vector<uint8_t> & buffer);

		/**
		 * \brief receives data on the socket
		 */
		ClockError read(std::string & buffer);	

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
		 *
		 * \param[in] fd
		 */
		TcpSocket(int fd);

		/**
		 * \brief reads platform specific error codes and returns a ClockError
		 */
		ClockError getLastError();

		/**
		 * \brief stores the lokal socket descriptor or -1 if not active
		 */
		int _sock;

		/**
		 * \brief current socket status
		 */
		SocketStatus _status;

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		static int _counter;
		static std::mutex _lock;
#endif

		/**
		 * \brief if a receivePacket gets more data than the packet contains, the rest is buffered in this variable
		 */
		std::vector<uint8_t> _buffer;

		TcpSocket(const TcpSocket &) = delete;
		TcpSocket & operator=(const TcpSocket &) = delete;
	};

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __SOCKETS_TCP_SOCKET_H__ */
