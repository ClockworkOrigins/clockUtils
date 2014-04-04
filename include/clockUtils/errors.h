#ifndef __CLOCKERRORS_H__
#define __CLOCKERRORS_H__

namespace clockUtils {

enum class ClockError {
	SUCCESS,
	// general
	NOT_READY,
	TIMEOUT,
	INVALID_USAGE,
	// sockets
	INVALID_IP,
	INVALID_PORT,
	ADDRESS_INUSE,
	CONNECTION_FAILED,
	NO_DATA,
	PORT_INUSE,
	//
	UNKNOWN,
	ERROR_COUNT
};

} /* namespace clockUtils */

#endif /* __CLOCKERRORS_H__ */
