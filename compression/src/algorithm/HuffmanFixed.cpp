/**
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can
 * redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#include "clockUtils/compression/algorithm/HuffmanFixed.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	std::vector<uint8_t> preVec({
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

	std::shared_ptr<HuffmanBase::Tree> HuffmanFixed::tree = HuffmanFixed::buildTree(preVec);
	std::map<uint8_t, std::pair<size_t, std::vector<uint8_t>>> HuffmanFixed::mappings = std::map<uint8_t, std::pair<size_t, std::vector<uint8_t>>>();

	std::string HuffmanFixed::compress(const std::string & text) {
		std::string result(5, 0x0);
		result[0] = char((((text.length() / 256) / 256) / 256) % 256);
		result[1] = char(((text.length() / 256) / 256) % 256);
		result[2] = char((text.length() / 256) % 256);
		result[3] = char(text.length() % 256);

		convert(text, 32, result);

		return result;
	}

	std::string HuffmanFixed::decompress(const std::string & text) {
		std::string realText(text.begin() + 4, text.end());

		size_t length = size_t(uint8_t(text[0]) * 256 * 256 * 256 + uint8_t(text[1]) * 256 * 256 + uint8_t(text[2]) * 256 + uint8_t(text[3]));

		std::string result(length, 0x0);

		getChar(realText, tree, length, result);

		return result;
	}

	void HuffmanFixed::convert(const std::string & text, size_t index, std::string & result) {
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

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
