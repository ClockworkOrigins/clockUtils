#ifndef __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANFIXED_H__
#define __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANFIXED_H__

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
		static std::map<unsigned char, std::pair<size_t, std::vector<uint8_t>>> mappings;
		
		/**
		 * \brief converts the text into a bit sequence using the mappings, if possible and otherwise filling the mappings
		 */
		static void convert(const std::string & text, size_t index, std::string & result);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANFIXED_H__ */
