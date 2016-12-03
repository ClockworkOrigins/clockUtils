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

#ifndef __CLOCKUTILS_LOG_LOG_H__
#define __CLOCKUTILS_LOG_LOG_H__

#include "clockUtils/log/Logger.h"
#include "clockUtils/log/LogLevel.h"

#define LOG(loglevel, message) \
	{\
		std::stringstream ss;\
		ss << message;\
		clockUtils::log::Logger::log(loglevel, ss.str());\
	}
	

#define LOGDEBUG(message) LOG(clockUtils::log::LogLevel::LOG_DEBUG, message)
#define LOGINFO(message) LOG(clockUtils::log::LogLevel::LOG_INFO, message)
#define LOGWARN(message) LOG(clockUtils::log::LogLevel::LOG_WARN, message)
#define LOGERROR(message) LOG(clockUtils::log::LogLevel::LOG_ERROR, message)
#define LOGFATAL(message) LOG(clockUtils::log::LogLevel::LOG_FATAL, message)

#endif /* __CLOCKUTILS_LOG_LOG_H__ */

/**
 * @}
 */
