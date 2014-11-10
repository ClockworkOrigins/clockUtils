#include "clockUtils/compression/algorithm/HuffmanGeneric.h"

#include <queue>

namespace clockUtils {
namespace compression {
namespace algorithm {

	std::string HuffmanGeneric::compress(const std::string & text) {
		std::vector<unsigned char> header = getHeader(text);

		std::string result(header.begin(), header.end());

		result += char((((text.length() / 256) / 256) / 256) % 256);
		result += char(((text.length() / 256) / 256) % 256);
		result += char((text.length() / 256) % 256);
		result += char(text.length() % 256);

		std::shared_ptr<Tree> tree = buildTree(header);

		std::string buffer = "";
		std::string newText = "";

		for (unsigned char c : text) {
			buffer += getBits(c, tree);

			while (buffer.length() >= 8) {
				newText += convertToChar(buffer.substr(0, 8));
				buffer = buffer.substr(8, buffer.length() - 8);
			}
		}

		if (!buffer.empty()) {
			newText += convertToChar(buffer);
		}

		return result + newText;
	}

	std::string HuffmanGeneric::decompress(const std::string & text) {
		std::vector<unsigned char> header(text.begin(), text.begin() + 256);

		std::shared_ptr<Tree> tree = buildTree(header);

		std::string realText(text.begin() + 260, text.end());

		int length = int(unsigned char(text[256]) * 256 * 256 * 256 + unsigned char(text[257]) * 256 * 256 + unsigned char(text[258]) * 256 + unsigned char(text[259]));

		std::string newText = "";
		std::string result;

		for (char c : realText) {
			newText += convertToBitString(c);
		}

		for (int i = 0; i < length; i++) {
			std::pair<unsigned char, int> p = getChar(newText, tree);
			result += p.first;
			newText = newText.substr(p.second, newText.length() - p.second);
		}

		return result;
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
