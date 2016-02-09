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

#ifndef __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANBASE_H__
#define __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANBASE_H__

#include <memory>
#include <string>
#include <vector>

#include "clockUtils/compression/compressionParameters.h"

namespace clockUtils {

	enum class ClockError;

namespace compression {
namespace algorithm {

	/**
	 * \brief base class for Huffman compression
	 * contains common methods and helper structures
	 */
	class HuffmanBase {
	protected:
		/**
		 * \brief type that should be used as the 'length' counter
		 * This controls the size of the header, the maximum string length etc.
		 */
		typedef uint32_t len_t;

		/**
		 * \brief represents a node in the probability tree
		 * if left and right are both nullptr this node is a leave and c has a valid value
		 */
		struct Node {
			len_t value;
			uint8_t c;
			std::shared_ptr<Node> left = nullptr;
			std::shared_ptr<Node> right = nullptr;
		};

		/**
		 * \brief creates a Huffman tree with given list of probabilities
		 * header must have size 256 and values have to be between 0 and 255 (unsigned char)
		 * if a value is zero no node is inserted into the tree and this value isn't accessible
		 */
		static std::shared_ptr<Node> buildTree(const std::vector<uint8_t> & header);

		/**
		 * \brief constructs a mapping from character to a bit sequence
		 * \param[in] node Root node of the probability tree
		 * \param[in] bitSeq Bit sequence until this point in the tree
		 * \param[out] mapping Constructed mapping
		 */
		static void generateMapping(const std::shared_ptr<Node> & node, const std::vector<bool> & bitSeq, std::vector<std::vector<bool>> & mapping);

		/**
		 * \brief converts bit sequence to the real character
		 * \param[in] compressed the compressed string containing the bit sequence
		 * \param[in] root the tree root with the encoding
		 * \param[in] length the amount of characters that are encoded in the compressed string
		 * \param[in,out] result the resulting string containing the decompressed string
		 */
		static ClockError getChar(const std::string & compressed, const std::shared_ptr<Node> & root, len_t length, std::string & result);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANBASE_H__ */

/**
 * @}
 */
