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
	 * \brief class for Huffman compression using a generic dictionary
	 */
	class CLOCK_COMPRESSION_API HuffmanGeneric {
	public:
		static std::string compress(const std::string & text);

		static std::string decompress(const std::string & text);

	private:
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

		static Tree * buildTree(const std::vector<unsigned char> & header);

		static std::string getBits(unsigned char c, Tree * tree);
		static std::string getBitsRec(unsigned char c, Tree * tree, const std::shared_ptr<Node> & node);

		static std::pair<unsigned char, int> getChar(const std::string & code, Tree * tree);

		static unsigned char convertToChar(const std::string & code);

		static std::string convertToBitString(unsigned char c);
	};

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
