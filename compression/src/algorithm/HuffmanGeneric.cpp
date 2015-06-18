/**
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "clockUtils/compression/algorithm/HuffmanGeneric.h"

#include <map>

namespace clockUtils {
namespace compression {
namespace algorithm {

	std::string HuffmanGeneric::compress(const std::string & text) {
		std::vector<uint8_t> header = getHeader(text);

		std::string result(header.begin(), header.end());

		result += char((((text.length() / 256) / 256) / 256) % 256);
		result += char(((text.length() / 256) / 256) % 256);
		result += char((text.length() / 256) % 256);
		result += char(text.length() % 256);
		result += char(uint8_t(0x0));

		std::shared_ptr<Tree> tree = buildTree(header);

		convert(text, tree, 260 * 8, result);

		return result;
	}

	std::string HuffmanGeneric::decompress(const std::string & text) {
		std::vector<uint8_t> header(text.begin(), text.begin() + 256);

		std::shared_ptr<Tree> tree = buildTree(header);

		std::string realText(text.begin() + 260, text.end());

		size_t length = size_t(uint8_t(text[256]) * 256 * 256 * 256 + uint8_t(text[257]) * 256 * 256 + uint8_t(text[258]) * 256 + uint8_t(text[259]));

		std::string result(length, 0x0);

		getChar(realText, tree, length, result);

		return result;
	}

	void HuffmanGeneric::convert(const std::string & text, const std::shared_ptr<Tree> & tree, size_t index, std::string & result) {
		std::map<uint8_t, std::pair<size_t, std::vector<uint8_t>>> mappings;
		for (uint8_t c : text) {
			auto it = mappings.find(c);

			if (it == mappings.end()) {
				size_t count = getBitsRec(c, tree, tree->left, 1, index, result);

				if (count == 0) {
					count = getBitsRec(c, tree, tree->right, 1, index, result);
				}

				std::vector<uint8_t> vec(count / 8 + 1, 0x0);

				for (size_t i = 0; i < count; i++) {
					vec[i / 8] += ((result[(index + i) / 8] & (1 << (7 - (index + i) % 8))) == (1 << (7 - (index + i) % 8))) ? (1 << (7 - i % 8)) : 0;
				}

				mappings.insert(std::make_pair(c, std::make_pair(count, vec)));

				index += count;
			} else {
				while ((index + it->second.first) / 8 + 1 > result.size()) {
					result += char(uint8_t(0x0));
				}

				for (size_t i = 0; i < it->second.first; i++) {
					result[(index + i) / 8] += ((it->second.second[i / 8] & (1 << (7 - i % 8))) == (1 << (7 - i % 8))) ? (1 << (7 - (index + i) % 8)) : 0;
				}

				index += it->second.first;
			}
		}
	}

	std::vector<uint8_t> HuffmanGeneric::getHeader(const std::string & text) {
		std::vector<int> header(256, 0);

		int max = 0;

		for (uint8_t c : text) {
			header[c]++;

			if (header[c] > max) {
				max = header[c];
			}
		}

		std::vector<uint8_t> charHeader(256, 0);

		for (size_t i = 0; i < 256; ++i) {
			charHeader[i] = uint8_t(header[i] / double(max) * 255.0);
			if (header[i] > 0 && charHeader[i] == 0) {
				charHeader[i] = 1;
			}
		}

		return charHeader;
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
