#include "clockUtils/sockets/TcpSocket.h"

namespace clockUtils {
namespace sockets {

TcpSocket::TcpSocket() : _sock(-1) {
}

TcpSocket::~TcpSocket() {
}

ClockError TcpSocket::getLastError() {
	return ClockError::UNKNOWN;
}

} /* namespace sockets */
} /* namespace clockUtils */
