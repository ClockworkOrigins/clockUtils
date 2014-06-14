#include "clockUtils/sockets/TcpSocket.h"

#include <chrono>
#include <thread>

#include "clockUtils/errors.h"

#include <WinSock2.h>


namespace clockUtils {
namespace sockets {

	int TcpSocket::_counter = 0;
	std::mutex TcpSocket::_lock;

	TcpSocket::TcpSocket() : _sock(-1), _status(SocketStatus::INACTIVE), _todoLock(), _todo(), _buffer(), _terminate(false), _worker(nullptr) {
		_lock.lock();
		if (_counter == 0) {
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 0), &wsa);
		}
		_counter++;
		_lock.unlock();

		_worker = new std::thread([this]() {
				while (!_terminate) {
					_todoLock.lock();
					while(_todo.size() > 0) {
						_todoLock.unlock();

						writePacket(const_cast<const unsigned char *>(&_todo.front()[0]), _todo.front().size());

						_todoLock.lock();
						_todo.pop();
					}
					_todoLock.unlock();
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			});
	}

	TcpSocket::~TcpSocket() {
		_terminate = true;
		_worker->join();
		delete _worker;
		close();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		_lock.lock();
		if (_counter == 1) {
			WSACleanup();
		}
		_counter++;
		_lock.unlock();
	}

	void TcpSocket::close() {
		if (_sock != -1) {
			shutdown(_sock, SD_BOTH);
			closesocket(_sock);
			_sock = -1;
			_status = SocketStatus::INACTIVE;
		}
	}

	ClockError TcpSocket::getLastError() {
		int error = WSAGetLastError();

		if (error == WSAEADDRINUSE) {
			return ClockError::ADDRESS_INUSE;
		} else if (error == WSA_INVALID_PARAMETER) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSA_INVALID_HANDLE) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSA_OPERATION_ABORTED) {
			return ClockError::NOT_READY;
		} else if (error == WSAEINTR) {
			return ClockError::NOT_READY;
		} else if (error == WSAEBADF) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEACCES) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEFAULT) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEINVAL) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEWOULDBLOCK) {
			return ClockError::IN_PROGRESS;
		} else if (error == WSAEINPROGRESS) {
			return ClockError::IN_PROGRESS;
		} else if (error == WSAEALREADY) {
			return ClockError::NOT_READY;
		} else if (error == WSAENOTSOCK) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAEDESTADDRREQ) {
			return ClockError::INVALID_IP;
		} else if (error == WSAEADDRNOTAVAIL) {
			return ClockError::INVALID_PORT;
		} else if (error == WSAENETDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAENETUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAENETUNREACH) {
			return ClockError::NOT_READY;
		} else if (error == WSAECONNRESET) {
			return ClockError::NOT_READY;
		} else if (error == WSAEISCONN) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAENOTCONN) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAESHUTDOWN) {
			return ClockError::INVALID_USAGE;
		} else if (error == WSAETIMEDOUT) {
			return ClockError::TIMEOUT;
		} else if (error == WSAECONNREFUSED) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAEHOSTDOWN) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSAEHOSTUNREACH) {
			return ClockError::CONNECTION_FAILED;
		} else if (error == WSASYSNOTREADY) {
			return ClockError::NOT_READY;
		} else if (error == WSAECONNABORTED) {
			return ClockError::NOT_CONNECTED;
		}

		return ClockError::UNKNOWN;
	}

} /* namespace sockets */
} /* namespace clockUtils */
