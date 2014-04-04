#include "clockUtils/sockets/TcpSocket.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <WinSock2.h>
#endif

namespace clockUtils {
namespace sockets {

	ClockError TcpSocket::listen(uint16_t listenPort, unsigned int maxParallelConnections, bool acceptMultiple, const acceptCallback & acb) {
		return ClockError::UNKNOWN;
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
			closesocket(_sock);
			_sock = -1;
			return ClockError::INVALID_IP;
		}

		int rc = ::connect(_sock, (sockaddr *) &addr, sizeof(sockaddr));

		if (rc == -1) {
			ClockError error = getLastError();
			closesocket(_sock);
			_sock = -1;
			return error;
		}

		return ClockError::SUCCESS;
	}

	void TcpSocket::close() {
		if (_sock != -1) {
			closesocket(_sock);
			_sock = -1;
		}
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
		hostent * localHost = gethostbyname("");
		char * localIP = inet_ntoa(*(struct in_addr *) *localHost->h_addr_list);

		std::string ip(localIP);

		return ip;
	}

	std::string TcpSocket::getPublicIP() const {
		if (_sock == -1) {
			return "";
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(sockaddr_in));
		int len = sizeof(addr);
		getsockname(_sock, (sockaddr *) &addr, &len);

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
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::read(std::vector<uint8_t> & buffer) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::read(std::string & buffer) {
		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
