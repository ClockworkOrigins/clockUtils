/**
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

#ifndef __CLOCKUTILS_COMPRESSION_COMPRESSION_H__
#define __CLOCKUTILS_COMPRESSION_COMPRESSION_H__

#include <string>

namespace clockUtils {
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
		std::string compress(const std::string & text) const {
			return Algorithm::compress(text);
		}

		/**
		 * \brief decompresses the given string and returns result
		 */
		std::string decompress(const std::string & text) const {
			return Algorithm::decompress(text);
		}
	};

} /* namespace compression */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_COMPRESSION_COMPRESSION_H__ */
