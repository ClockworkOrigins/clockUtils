/*
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * \addtogroup log
 * @{
 */

#ifndef __CLOCKUTILS_LOG_LOGGER_H__
#define __CLOCKUTILS_LOG_LOGGER_H__

#include <algorithm>
#include <set>

#include "clockUtils/errors.h"
#include "clockUtils/log/LogParameters.h"
#include "clockUtils/log/SinkWrapper.h"

namespace clockUtils {
namespace log {

	enum class LogLevel;

namespace detail {
	template<typename S, typename T>
	class isStreamable {
	public:
		template<typename SS, typename TT>
		static auto test(int) -> decltype(std::declval<SS &>() << std::declval<TT>(), std::true_type());

		template<typename, typename>
		static auto test(...) -> std::false_type;

		static const bool value = decltype(test<S, T>(0))::value;
	};
} /* namespace detail */

	/**
	 * \brief this class is used for parsing configuration files
	 */
	class CLOCK_LOG_API Logger {
	public:
		template<typename T>
		static typename std::enable_if<detail::isStreamable<T, std::string>::value, ClockError>::type addSink(T * sink) {
			if (sinks.end() == std::find_if(sinks.begin(), sinks.end(), [sink](SinkWrapper * sw) { return sw->isSame(sink); })) {
				SinkWrapper * sinkWrapper = new SinkWrapperImplementation<T>(sink);
				sinks.insert(sinkWrapper);
				return ClockError::SUCCESS;
			}
			return ClockError::SINK_ALREADY_INSERTED;
		}

		template<typename T>
		static typename std::enable_if<!detail::isStreamable<T, std::string>::value, ClockError>::type addSink(T *) {
			return ClockError::NO_VALID_SINK;
		}

		static void log(LogLevel level, const std::string & message);
		static void reset();
		static void setLogLevel(LogLevel level);

	private:
		static std::set<SinkWrapper *> sinks;
		static LogLevel currentLogLevel;
	};

} /* namespace log */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_LOG_LOGGER_H__ */

/**
 * @}
 */
