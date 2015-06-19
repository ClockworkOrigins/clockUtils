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

#ifndef __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANGENERIC_H__
#define __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANGENERIC_H__

#include "clockUtils/compression/algorithm/HuffmanBase.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	/**
	 * \brief class for Huffman compression using a dictionary generated out of the given string
	 * has always an overhead of 256 Bytes because of the header
	 */
	class CLOCK_COMPRESSION_API HuffmanGeneric : public HuffmanBase {
	public:
		/**
		 * \brief compresses the given string and returns result
		 */
		static std::string compress(const std::string & text);

		/**
		 * \brief decompresses the given string and returns result
		 */
		static std::string decompress(const std::string & text);

	private:
		/**
		 * \brief converts the given text to a bit sequence using a cache internally to improve performance
		 */
		static void convert(const std::string & text, const std::shared_ptr<Tree> & tree, size_t index, std::string & result);

		/**
		 * \brief calculates the probabilities used for the tree and returns it
		 */
		static std::vector<uint8_t> getHeader(const std::string & text);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANGENERIC_H__ */

/**
 * @}
 */
