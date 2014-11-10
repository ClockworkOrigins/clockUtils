#include "clockUtils/compression/algorithm/HuffmanFixed.h"

#include <queue>

namespace clockUtils {
namespace compression {
namespace algorithm {

	std::vector<unsigned char> vec({
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		7, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 255, 1, 1, 1, 1, 1, 1, 3,
		4, 4, 1, 1, 10, 2, 10, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
		1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 3, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 74, 24, 30,
		40, 179, 24, 23, 67, 95, 1, 3, 50, 33,
		94, 88, 20, 8, 66, 69, 111, 33, 7, 21,
		2, 21, 2, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1
	});

	std::shared_ptr<HuffmanBase::Tree> HuffmanFixed::tree = HuffmanFixed::buildTree(vec);
	std::map<unsigned char, std::string> HuffmanFixed::mappings = std::map<unsigned char, std::string>();

	std::string HuffmanFixed::compress(const std::string & text) {
		std::string result;
		result += char((((text.length() / 256) / 256) / 256) % 256);
		result += char(((text.length() / 256) / 256) % 256);
		result += char((text.length() / 256) % 256);
		result += char(text.length() % 256);

		std::string buffer = "";
		std::string newText = "";

		for (unsigned char c : text) {
			auto it = mappings.find(c);

			if (it != mappings.end()) {
				buffer += it->second;
			} else {
				std::string tmp = getBits(c, tree);
				buffer += tmp;
				mappings.insert(std::make_pair(c, tmp));
			}

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

	std::string HuffmanFixed::decompress(const std::string & text) {
		std::string realText(text.begin() + 4, text.end());

		int length = int(unsigned char(text[0]) * 256 * 256 * 256 + unsigned char(text[1]) * 256 * 256 + unsigned char(text[2]) * 256 + unsigned char(text[3]));

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
