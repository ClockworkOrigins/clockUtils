#include "clockUtils/sockets/TcpSocket.h"

#define _GLIBCXX_USE_NANOSLEEP // needed for sleep_for, see http://stackoverflow.com/questions/4438084/stdthis-threadsleep-for-and-gcc

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	#include <sys/socket.h>
	#include <unistd.h>
#endif

#include <chrono>
#include <thread>

namespace clockUtils {
namespace sockets {

	TcpSocket::TcpSocket() : _sock(-1) {
	}

	TcpSocket::~TcpSocket() {
		close();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	void TcpSocket::close() {
		if (_sock != -1) {
			// needed to stop pending accept operations
			::shutdown(_sock, SHUT_RDWR); // FIXME: only do this if connected?, check for errorcode than
			if (-1 == ::close(_sock)) perror("close");
			_sock = -1;
		}
	}

	ClockError TcpSocket::getLastError() {
		if (errno == EADDRINUSE) {
			return ClockError::ADDRESS_INUSE;
		} else if (errno == EINTR) {
			return ClockError::NOT_READY;
		} else if (errno == EBADF) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EACCES) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EFAULT) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EINVAL) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EWOULDBLOCK) {
			return ClockError::NODATA;
		} else if (errno == EINPROGRESS) {
			return ClockError::IN_PROGRESS;
		} else if (errno == EALREADY) {
			return ClockError::NOT_READY;
		} else if (errno == ENOTSOCK) {
			return ClockError::INVALID_USAGE;
		} else if (errno == EDESTADDRREQ) {
			return ClockError::INVALID_IP;
		} else if (errno == EADDRNOTAVAIL) {
			return ClockError::INVALID_PORT;
		} else if (errno == ENETDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == ENETUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == ENETUNREACH) {
			return ClockError::NOT_READY;
		} else if (errno == ECONNRESET) {
			return ClockError::NOT_READY;
		} else if (errno == EISCONN) {
			return ClockError::INVALID_USAGE;
		} else if (errno == ENOTCONN) {
			return ClockError::INVALID_USAGE;
		} else if (errno == ESHUTDOWN) {
			return ClockError::INVALID_USAGE;
		} else if (errno == ETIMEDOUT) {
			return ClockError::TIMEOUT;
		} else if (errno == ECONNREFUSED) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == EHOSTDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (errno == EHOSTUNREACH) {
			return ClockError::CONNECTION_FAILED;
		}
		
		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
