#include "clockUtils/sockets/TcpSocket.h"

namespace clockUtils {
namespace sockets {

	ClockError TcpSocket::listen(uint16_t listenPort, unsigned int maxParallelConnections, bool acceptMultiple, const acceptCallback & acb) {
		return ClockError::UNKNOWN;
	}

	ClockError TcpSocket::connect(const std::string & remoteIP, uint16_t remotePort, unsigned int timeout) {
		return ClockError::UNKNOWN;
	}

	void TcpSocket::close() {
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
		return "NOT IMPLEMENTED YET";
	}

	std::string TcpSocket::getPublicIP() const {
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
