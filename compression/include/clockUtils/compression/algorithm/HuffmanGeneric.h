#ifndef __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANGENERIC_H__
#define __CLOCKUTILS_COMPRESSION_ALGORITHM_HUFFMANGENERIC_H__

#include "clockUtils/compression/algorithm/HuffmanBase.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	/**
	 * \brief class for Huffman compression using a generic dictionary
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
