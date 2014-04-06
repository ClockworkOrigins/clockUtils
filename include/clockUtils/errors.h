#ifndef __CLOCKUTILS_CLOCKERRORS_H__
#define __CLOCKUTILS_CLOCKERRORS_H__

namespace clockUtils {

	enum class ClockError {
		SUCCESS,
		// general
		NOT_READY,
		TIMEOUT,
		INVALID_ARGUMENT,
		INVALID_USAGE,
		IN_PROGRESS,
		// sockets
		INVALID_IP,
		INVALID_PORT,
		ADDRESS_INUSE,
		CONNECTION_FAILED,
		NOT_CONNECTED,
		//
		UNKNOWN,
		ERROR_COUNT
	};

} /* namespace clockUtils */

#endif /* __CLOCKUTILS_CLOCKERRORS_H__ */
