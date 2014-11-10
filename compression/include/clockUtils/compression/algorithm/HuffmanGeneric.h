#pragma once

#include "clockUtils/compression/algorithm/HuffmanBase.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	/**
	 * \brief class for Huffman compression using a generic dictionary
	 */
	class CLOCK_COMPRESSION_API HuffmanGeneric : public HuffmanBase {
	public:
		static std::string compress(const std::string & text);

		static std::string decompress(const std::string & text);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
