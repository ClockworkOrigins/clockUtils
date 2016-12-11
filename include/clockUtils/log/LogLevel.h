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

/**
 * \addtogroup log
 * @{
 */

#ifndef __CLOCKUTILS_LOG_LOGLEVEL_H__
#define __CLOCKUTILS_LOG_LOGLEVEL_H__

namespace clockUtils {
namespace log {

	/**
	 * \brief this class is used for parsing configuration files
	 */
	enum class LogLevel {
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR,
		LOG_FATAL
	};

} /* namespace log */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_LOG_LOGLEVEL_H__ */

/**
 * @}
 */
