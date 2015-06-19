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
namespace compression {
namespace algorithm {

	/**
	 * \brief base class for Huffman compression
	 * contains common methods and helper structures
	 */
	class HuffmanBase {
	protected:
		/**
		 * \brief represents a node in the probability graph
		 * if left and right are both nullptr this node is a leave and c has a valid value
		 * if parent is nullptr this nodes parent is the tree itself
		 */
		struct Node {
			int value;
			uint8_t c;
			std::shared_ptr<Node> parent = nullptr;
			std::shared_ptr<Node> left = nullptr;
			std::shared_ptr<Node> right = nullptr;
		};

		/**
		 * \brief represents the tree and is used a the root
		 */
		struct Tree {
			std::shared_ptr<Node> left = nullptr;
			std::shared_ptr<Node> right = nullptr;

			~Tree();

		private:
			void deleteHelper(const std::shared_ptr<Node> & node);
		};

		/**
		 * \brief creates a Huffman tree with given list of probabilities
		 * header must have size 256 and values have to be between 0 and 255 (unsigned char)
		 * if a value is zero no node is inserted into the tree and this value isn't accessible
		 */
		static std::shared_ptr<Tree> buildTree(const std::vector<uint8_t> & header);

		/**
		 * \brief recursiv helper function to find bits for given character
		 * \param[in] c character to be encoded
		 * \param[in] tree the tree with the encoding
		 * \param[in] node the current node to be checked
		 * \param[in] count the amount of recursion steps currently taken, necessary to identify position and to know how many bytes have to be allocated to contain all new bits
		 * \param[in] index index of the bit found during this call
		 * \param[in,out] result the resulting string containing the whole bit sequence
		 * \return returns the count of the leave node found or 0, if current leave doesn't match c
		 */
		static size_t getBitsRec(uint8_t c, const std::shared_ptr<Tree> & tree, const std::shared_ptr<Node> & node, size_t count, size_t index, std::string & result);

		/**
		 * \brief converts bit sequence to the real character
		 * \param[in] compressed the compressed string containing the bit sequence
		 * \param[in] tree the tree with the encoding
		 * \param[in] length the amount of characters that are encoded in the compressed string
		 * \param[in,out] result the resulting string containing the decompressed string
		 */
		static void getChar(const std::string & compressed, const std::shared_ptr<Tree> & tree, size_t length, std::string & result);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANBASE_H__ */

/**
 * @}
 */
