#include "clockUtils/compression/algorithm/HuffmanGeneric.h"

#include <map>

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
		result += char(unsigned char(0x0));

		std::shared_ptr<Tree> tree = buildTree(header);

		convert(text, tree, 260 * 8, result);

		return result;
	}

	std::string HuffmanGeneric::decompress(const std::string & text) {
		std::vector<unsigned char> header(text.begin(), text.begin() + 256);

		std::shared_ptr<Tree> tree = buildTree(header);

		std::string realText(text.begin() + 260, text.end());

		size_t length = size_t(unsigned char(text[256]) * 256 * 256 * 256 + unsigned char(text[257]) * 256 * 256 + unsigned char(text[258]) * 256 + unsigned char(text[259]));

		std::string result(length, 0x0);

		getChar(realText, tree, length, result);

		return result;
	}

	void HuffmanGeneric::convert(const std::string & text, const std::shared_ptr<Tree> & tree, size_t index, std::string & result) {
		std::map<unsigned char, std::pair<size_t, std::vector<unsigned char>>> mappings;
		for (unsigned char c : text) {
			auto it = mappings.find(c);

			if (it == mappings.end()) {
				size_t count = getBitsRec(c, tree, tree->left, 1, index, result);

				if (count == 0) {
					count = getBitsRec(c, tree, tree->right, 1, index, result);
				}

				std::vector<unsigned char> vec(count / 8 + 1, 0x0);

				for (size_t i = 0; i < count; i++) {
					vec[i / 8] += ((result[(index + i) / 8] & (1 << (7 - (index + i) % 8))) == (1 << (7 - (index + i) % 8))) ? (1 << (7 - i % 8)) : 0;
				}

				mappings.insert(std::make_pair(c, std::make_pair(count, vec)));

				index += count;
			} else {
				while ((index + it->second.first) / 8 + 1 > result.size()) {
					result += char(unsigned char(0x0));
				}

				for (size_t i = 0; i < it->second.first; i++) {
					result[(index + i) / 8] += ((it->second.second[i / 8] & (1 << (7 - i % 8))) == (1 << (7 - i % 8))) ? (1 << (7 - (index + i) % 8)) : 0;
				}

				index += it->second.first;
			}
		}
	}

	std::vector<unsigned char> HuffmanGeneric::getHeader(const std::string & text) {
		std::vector<int> header(256, 0);

		int max = 0;

		for (unsigned char c : text) {
			header[c]++;

			if (header[c] > max) {
				max = header[c];
			}
		}

		std::vector<unsigned char> charHeader(256, 0);

		for (size_t i = 0; i < 256; ++i) {
			charHeader[i] = unsigned char(header[i] / double(max) * 255.0);
			if (header[i] > 0 && charHeader[i] == 0) {
				charHeader[i] = 1;
			}
		}

		return charHeader;
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
