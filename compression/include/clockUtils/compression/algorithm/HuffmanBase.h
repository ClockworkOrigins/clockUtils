#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "clockUtils/compression/compressionParameters.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	/**
	 * \brief base class for Huffman compression
	 */
	class CLOCK_COMPRESSION_API HuffmanBase {
	protected:
		struct Node {
			int value;
			unsigned char c;
			std::shared_ptr<Node> parent = nullptr;
			std::shared_ptr<Node> left = nullptr;
			std::shared_ptr<Node> right = nullptr;
		};
		struct Tree {
			std::shared_ptr<Node> left = nullptr;
			std::shared_ptr<Node> right = nullptr;
		};
		static std::vector<unsigned char> getHeader(const std::string & text);

		static std::shared_ptr<Tree> buildTree(const std::vector<unsigned char> & header);

		static std::string getBits(unsigned char c, const std::shared_ptr<Tree> & tree);
		static std::string getBitsRec(unsigned char c, const std::shared_ptr<Tree> & tree, const std::shared_ptr<Node> & node);

		static void getChar(const std::string & compressed, const std::shared_ptr<HuffmanBase::Tree> & tree, size_t & index, std::string & result);

		static unsigned char convertToChar(const std::string & code);

		static std::string convertToBitString(unsigned char c);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
