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

#ifndef __CLOCKUTILS_LOG_SINKWRAPPER_H__
#define __CLOCKUTILS_LOG_SINKWRAPPER_H__

#include <string>

#include "clockUtils/log/LogParameters.h"

namespace clockUtils {
namespace log {

	/**
	 * \brief this class is used for parsing configuration files
	 */
	class SinkWrapper {
	public:
		virtual ~SinkWrapper() {
		}

		virtual SinkWrapper & operator<<(const std::string &) = 0;

		virtual bool isSame(void * sink) const = 0;
	};

	template<typename T>
	class SinkWrapperImplementation : public SinkWrapper {
	public:
		SinkWrapperImplementation(T * sink) : SinkWrapper(), _sink(sink) {
		}

		SinkWrapper & operator<<(const std::string & message) override {
			*_sink << message;
			return *this;
		}

		bool isSame(void * sink) const override {
			return sink == _sink;
		}

	private:
		T * _sink;
	};

} /* namespace log */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_LOG_SINKWRAPPER_H__ */

/**
 * @}
 */
