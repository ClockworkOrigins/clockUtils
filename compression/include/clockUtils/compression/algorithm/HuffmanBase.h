#pragma once

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
		 * \params[in] c character to be encoded
		 * \params[in] tree the tree with the encoding
		 * \params[in] node the current node to be checked
		 * \params[in] count the amount of recursion steps currently taken, necessary to identify position and to know how many bytes have to be allocated to contain all new bits
		 * \params[in] index index of the bit found during this call
		 * \params[in/out] result the resulting string containing the whole bit sequence
		 * \return returns the count of the leave node found or 0, if current leave doesn't match c
		 */
		static size_t getBitsRec(uint8_t c, const std::shared_ptr<Tree> & tree, const std::shared_ptr<Node> & node, size_t count, size_t index, std::string & result);

		/**
		 * \brief converts bit sequence to the real character
		 * \params[in] compressed the compressed string containing the bit sequence
		 * \params[in] tree the tree with the encoding
		 * \params[in] length the amount of characters that are encoded in the compressed string
		 * \params[in/out] result the resulting string containing the decompressed string
		 */
		static void getChar(const std::string & compressed, const std::shared_ptr<Tree> & tree, size_t length, std::string & result);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
