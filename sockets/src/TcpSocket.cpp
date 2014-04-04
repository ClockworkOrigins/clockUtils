#include "clockUtils/sockets/TcpSocket.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <WinSock2.h>
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	#include <arpa/inet.h>
	#include <cstring>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif
	#include <errno.h>
#include <thread>

namespace clockUtils {
namespace sockets {

	TcpSocket::TcpSocket(int fd) : TcpSocket() {
		_sock = fd;
	}

	ClockError TcpSocket::listen(uint16_t listenPort, int maxParallelConnections, bool acceptMultiple, const acceptCallback acb) {
		if (listenPort == 0) {
			return ClockError::INVALID_PORT;
		}
		if (_sock != -1) {
			return ClockError::INVALID_USAGE;
		}
		if (maxParallelConnections < 0) {
			return ClockError::INVALID_ARGUMENT;
		}
		_sock = socket(PF_INET, SOCK_STREAM, 0);
		if (-1 == _sock) {
			return ClockError::CONNECTION_FAILED;
		}

		// set reusable
		int flag = 1;
		setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

		struct sockaddr_in name = {AF_INET, htons(listenPort), INADDR_ANY, {0} };
		errno = 0;
		if (-1 == bind(_sock, reinterpret_cast<struct sockaddr *>(&name), sizeof(name))) {
			ClockError error = getLastError();
			close();
			return error;
		}

		errno = 0;
		if (-1 == ::listen(_sock, maxParallelConnections)) {
			ClockError error = getLastError();
			close();
			return error;
		}

		std::thread thrd([acceptMultiple, acb, this]()
			{
				if (acceptMultiple) {
					while(1) {
						errno = 0;
						int clientSock = ::accept(_sock, nullptr, nullptr);
						if (clientSock == -1) {
							close();
							return;
						}
						std::thread thrd2(std::bind(acb, new TcpSocket(clientSock)));
						thrd2.detach();
					}
				} else {
					int clientSock = ::accept(_sock, nullptr, nullptr);
					if (clientSock == -1) {
						close();
						return;
					}
					acb(new TcpSocket(clientSock));
				}
			});
		thrd.detach();

		return ClockError::SUCCESS;
	}

	ClockError TcpSocket::connect(const std::string & remoteIP, uint16_t remotePort, unsigned int timeout) {
		if (_sock != -1) {
			return ClockError::INVALID_USAGE;
		}

		if (remotePort == 0) {
			return ClockError::INVALID_PORT;
		}

		if (remoteIP.length() < 8) {
			return ClockError::INVALID_IP;
		}

		_sock = socket(PF_INET, SOCK_STREAM, 0);

		if (_sock == -1) {
			_sock = -1;

			return getLastError();
		}

		sockaddr_in addr;

		memset(&addr, 0, sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(remotePort);
		addr.sin_addr.s_addr = inet_addr(remoteIP.c_str());

		if (addr.sin_addr.s_addr == INADDR_NONE || addr.sin_addr.s_addr == INADDR_ANY) {
			close();
			return ClockError::INVALID_IP;
		}

		// set socket non-blockable
		long arg = fcntl(_sock, F_GETFL, NULL); 
		arg |= O_NONBLOCK; 
		fcntl(_sock, F_SETFL, arg);
		
		// connect
		if (-1 == ::connect(_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr))) {
			ClockError error = getLastError();
			if (error == ClockError::IN_PROGRESS) {
				// connect still in progress. wait for completion with timeout
				struct timeval tv;
				fd_set myset;
				tv.tv_sec = timeout / 1000;
				tv.tv_usec = timeout % 1000;
				FD_ZERO(&myset); 
				FD_SET(_sock, &myset); 
				if (select(_sock+1, NULL, &myset, NULL, &tv) > 0) { 
					socklen_t lon;
					lon = sizeof(int); 
					int valopt;
					getsockopt(_sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
					if (valopt) {
						close();
						return ClockError::CONNECTION_FAILED;
					}
				} else { 
					close();
					return ClockError::TIMEOUT;
				} 
			} else {
				close();
				return error;
			}
		}

		return ClockError::SUCCESS;
	}

	std::string TcpSocket::getRemoteIP() const {
		return "NOT IMPLEMENTED YET";
	}

	uint16_t TcpSocket::getRemotePort() const {
		return 0;
	}

	std::vector<std::pair<std::string, std::string>> TcpSocket::enumerateLocalIPs() {
		return std::vector<std::pair<std::string, std::string>>();
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
		if (_sock == -1) {
			return "";
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(sockaddr_in));
		unsigned int len = sizeof(addr);
		getsockname(_sock, reinterpret_cast<sockaddr *>(&addr), &len);

		return "NOT IMPLEMENTED YET";
	}

	uint16_t TcpSocket::getLocalPort() const {
		return 0;
	}

	ClockError TcpSocket::writePacket(const char * str, const size_t length) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::writePacket(const std::vector<uint8_t> & str) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::receivePacket(std::vector<uint8_t> & buffer) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::receivePacket(std::string & buffer) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::write(const void * str, size_t length) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::write(const std::vector<uint8_t> & str) {
		return write(const_cast<const unsigned char *>(&str[0]), str.size());
	}

	ClockError TcpSocket::read(std::vector<uint8_t> & buffer) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::read(std::string & buffer) {
		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
