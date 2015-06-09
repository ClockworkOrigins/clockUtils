#ifndef __CLOCKUTILS_CLOCKERRORS_H__
#define __CLOCKUTILS_CLOCKERRORS_H__

namespace clockUtils {

	enum class ClockError {
		SUCCESS,
		// general
		NOT_READY,
		TIMEOUT,
		INVALID_ARGUMENT,	// !< parameters are invalid
		INVALID_USAGE,		// !< library is used in wrong way
		IN_PROGRESS,
		WRONG_TYPE,			// !< value couldn't be cast in expected type
		WRONG_SYNTAX,		// !< syntax of parsed file is not as expected
		FILENOTFOUND,		// !< file does not exist
		// sockets
		INVALID_IP,
		INVALID_PORT,
		ADDRESS_INUSE,
		CONNECTION_FAILED,
		NOT_CONNECTED,
		// iniParser
		VALUE_NOTFOUND,		// !< value not found
		//
		UNKNOWN,
		ERROR_COUNT
	};

} /* namespace clockUtils */

#endif /* __CLOCKUTILS_CLOCKERRORS_H__ */
