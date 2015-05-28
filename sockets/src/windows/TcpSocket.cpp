#include "clockUtils/sockets/TcpSocket.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "clockUtils/errors.h"

namespace clockUtils {
namespace sockets {

	class WSAHelper {
	public:
		WSAHelper() {
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 0), &wsa);
		}

		~WSAHelper() {
			WSACleanup();
		}
	};

	TcpSocket::TcpSocket() : _sock(-1), _status(SocketStatus::INACTIVE), _todoLock(), _todo(), _buffer(), _terminate(false), _worker(nullptr), _listenThread(nullptr), _objCondExecutable(), _objCondMut(), _objCondUniqLock(_objCondMut), _callbackThread(nullptr) {
		static WSAHelper wsa;
		_worker = new std::thread([this]() {
				while (!_terminate) {
					_todoLock.lock();
					while (_todo.size() > 0) {
						std::vector<uint8_t> tmp = std::move(_todo.front());
						_todoLock.unlock();

						writePacket(const_cast<const unsigned char *>(&tmp[0]), tmp.size());

						_todoLock.lock();
						_todo.pop();
					}
					_todoLock.unlock();
					_objCondExecutable.wait(_objCondUniqLock);
				}
			});
	}

	TcpSocket::~TcpSocket() {
		_terminate = true;
		_objCondExecutable.notify_all();
		if (_worker->joinable()) {
			_worker->join();
		}
		delete _worker;
		close();
		if (_listenThread) {
			if (_listenThread->joinable()) {
				_listenThread->join();
			}
			delete _listenThread;
		}
	}

	void TcpSocket::close() {
		if (_sock != -1) {
			shutdown(_sock, SD_BOTH);
			closesocket(_sock);
			_sock = -1;
			_status = SocketStatus::INACTIVE;
		}
		try {
			if (_callbackThread != nullptr) {
				if (_callbackThread->joinable()) {
					_callbackThread->join();
				}
				delete _callbackThread;
				_callbackThread = nullptr;
			}
		} catch (std::system_error &) {
			// this can only be a deadlock, so do nothing here and delete thread in destructor
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
