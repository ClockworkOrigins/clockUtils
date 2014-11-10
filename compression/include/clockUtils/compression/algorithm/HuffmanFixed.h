#pragma once

#include "clockUtils/compression/algorithm/HuffmanBase.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	/**
	 * \brief class for Huffman compression using a fixed dictionary
	 */
	class CLOCK_COMPRESSION_API HuffmanFixed : public HuffmanBase {
	public:
		static std::string compress(const std::string & text);

		static std::string decompress(const std::string & text);

	private:
		static std::shared_ptr<Tree> tree;
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
