#pragma once

#include <map>

#include "clockUtils/compression/algorithm/HuffmanBase.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	/**
	 * \brief class for Huffman compression using a fixed dictionary
	 */
	class CLOCK_COMPRESSION_API HuffmanFixed : public HuffmanBase {
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
		 * \brief the tree in fixed Huffman is always the same, so it can be stored as static member during whole lifetime
		 */
		static std::shared_ptr<Tree> tree;

		/**
		 * \brief because the tree in fixed Huffman stays always the same, also the mappings are constant
		 */
		static std::map<unsigned char, std::pair<size_t, std::vector<unsigned char>>> mappings; 
		
		/**
		 * \brief converts the text into a bit sequence using the mappings, if possible and otherwise filling the mappings
		 */
		static void convert(const std::string & text, const std::shared_ptr<Tree> & tree, size_t index, std::string & result);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
