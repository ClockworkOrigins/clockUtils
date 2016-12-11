/*
 * clockUtils
 * Copyright (2016) Michael Baer, Daniel Bonrath, All rights reserved.
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

#include "clockUtils/log/Logger.h"

#include "clockUtils/log/LogLevel.h"

namespace clockUtils {
namespace log {

	std::set<SinkWrapper *> Logger::sinks = std::set<SinkWrapper *>();
	LogLevel Logger::currentLogLevel = LogLevel::LOG_DEBUG;

	void Logger::log(LogLevel level, const std::string & message) {
		if (level >= currentLogLevel) {
			for (SinkWrapper * sw : sinks) {
				*sw << message << "\n";
			}
		}
	}

	void Logger::reset() {
		for (SinkWrapper * sw : sinks) {
			delete sw;
		}
		sinks.clear();
	}

	void Logger::setLogLevel(LogLevel level) {
		currentLogLevel = level;
	}

} /* namespace log */
} /* namespace clockUtils */
