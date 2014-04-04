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

	TcpSocket();

	~TcpSocket();

	ClockError listen(uint16_t listenPort, int maxParallelConnections, bool acceptMultiple, const acceptCallback acb);
	
	ClockError connect(const std::string & remoteIP, uint16_t remotePort, unsigned int timeout);
	
	void close();
	
	std::string getRemoteIP() const;
	
	uint16_t getRemotePort() const;
	
	static std::vector<std::pair<std::string, std::string>> enumerateLocalIPs();

	std::string getLocalIP() const;

	std::string getPublicIP() const;

	uint16_t getLocalPort() const;
	
	ClockError writePacket(const char * str, const size_t length);
	
	ClockError writePacket(const std::vector<uint8_t> & str);

	ClockError receivePacket(std::vector<uint8_t> & buffer);

	ClockError receivePacket(std::string & buffer);

	ClockError write(const void * str, size_t length);
	
	ClockError write(const std::vector<uint8_t> & str);
	
	ClockError read(std::vector<uint8_t> & buffer);

	ClockError read(std::string & buffer);	

	/* void operator<<(int a);
	
	void operator>>(int & a); */

private:
	TcpSocket(int fd);

	ClockError getLastError();

	/**
	 * \brief stores the lokal socket descriptor or -1 if not active
	 */
	int _sock;

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	static int _counter;
	static std::mutex _lock;
#endif

	TcpSocket(const TcpSocket &) = delete;
	TcpSocket & operator=(const TcpSocket &) = delete;
};

} /* namespace sockets */
} /* namespace clockUtils */

#endif /* __SOCKETS_TCP_SOCKET_H__ */
