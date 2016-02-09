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
 * \addtogroup compression
 * @{
 */

#ifndef __CLOCKUTILS_COMPRESSION_COMPRESSION_H__
#define __CLOCKUTILS_COMPRESSION_COMPRESSION_H__

#include <string>

namespace clockUtils {

	enum class ClockError;

namespace compression {

	/**
	 * \brief class for string compression and decompression using Algorithm as template parameter
	 */
	template<typename Algorithm>
	class Compression {
	public:
		/**
		 * \brief compresses the given string and returns result
		 */
		ClockError compress(const std::string & uncompressed, std::string & compressed) const {
			return Algorithm::compress(uncompressed, compressed);
		}

		/**
		 * \brief decompresses the given string and returns result
		 */
		ClockError decompress(const std::string & compressed, std::string & decompressed) const {
			return Algorithm::decompress(compressed, decompressed);
		}
	};

} /* namespace compression */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_COMPRESSION_COMPRESSION_H__ */

/**
 * @}
 */
