#include "clockUtils/sockets/UdpSocket.h"

#include <errno.h>
#include <iostream>
#include <thread>

namespace clockUtils {
namespace sockets {

	UdpSocket::UdpSocket(int fd) : UdpSocket() {
		_sock = fd;
	}

	ClockError UdpSocket::bind(uint16_t port) {
		if (_sock != -1) {
			return ClockError::INVALID_USAGE;
		}

		struct sockaddr_in si;

		if ((_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
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

	std::string UdpSocket::getRemoteIP() const {
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getpeername(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return inet_ntoa(localAddress.sin_addr);
	}

	uint16_t UdpSocket::getRemotePort() const {
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getpeername(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return static_cast<uint16_t>(ntohs(localAddress.sin_port));
	}

	std::vector<std::pair<std::string, std::string>> UdpSocket::enumerateLocalIPs() {
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

	std::string UdpSocket::getLocalIP() const {
		char buffer[256];
		gethostname(buffer, 256);
		hostent * localHost = gethostbyname(buffer);
		char * localIP = inet_ntoa(*reinterpret_cast<struct in_addr *>(*localHost->h_addr_list));

		std::string ip(localIP);

		return ip;
	}

	std::string UdpSocket::getPublicIP() const {
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getsockname(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return inet_ntoa(localAddress.sin_addr);
	}

	uint16_t UdpSocket::getLocalPort() const {
		struct sockaddr_in localAddress;
		socklen_t addressLength = sizeof(localAddress);
		getsockname(_sock, reinterpret_cast<struct sockaddr *>(&localAddress), &addressLength);
		return static_cast<uint16_t>(ntohs(localAddress.sin_port));
	}

	ClockError UdpSocket::writePacket(const std::string & ip, uint16_t port, const void * str, const uint32_t length) {
		if (_sock == -1) {
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

	ClockError UdpSocket::writePacket(const std::string & ip, uint16_t port, const std::vector<uint8_t> & str) {
		return writePacket(ip, port, const_cast<const unsigned char *>(&str[0]), str.size());
	}

	ClockError UdpSocket::write(const std::string & ip, uint16_t port, const void * str, uint32_t length) {
		if (_sock == -1) {
			return ClockError::NOT_READY;
		}
		struct sockaddr_in addr;

		memset(reinterpret_cast<char *>(&addr), 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());

		for (int i = 0; i < length / MAX_PACKET_SIZE + 1; i++) {
			uint32_t sendLength = (i < length / MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : length - (i * MAX_PACKET_SIZE);
			if (sendto(_sock, &reinterpret_cast<const char *>(str)[i * MAX_PACKET_SIZE], sendLength, 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
				ClockError error = getLastError();
				return error;
			}
		}
		return ClockError::SUCCESS;
	}

	ClockError UdpSocket::receivePacket(std::vector<uint8_t> & buffer, std::string & ip, uint16_t & port) {
		if (_sock == -1) {
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
		if (_sock == -1) {
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
					length = static_cast<uint32_t>(result[1] * 256 * 256 * 256 + result[2] * 256 * 256 + result[3] * 256 + result[4]);
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

} /* namespace sockets */
} /* namespace clockUtils */
