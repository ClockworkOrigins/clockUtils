/*
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

#include "clockUtils/compression/algorithm/HuffmanFixed.h"

#include "clockUtils/errors.h"

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

	ClockError HuffmanFixed::compress(const std::string & uncompressed, std::string & compressed) {
		if (uncompressed.length() > INT32_MAX / 2) {
			return ClockError::INVALID_ARGUMENT;
		}
		compressed = std::string(5, 0x0);
		compressed[0] = char((((uncompressed.length() / 256) / 256) / 256) % 256);
		compressed[1] = char(((uncompressed.length() / 256) / 256) % 256);
		compressed[2] = char((uncompressed.length() / 256) % 256);
		compressed[3] = char(uncompressed.length() % 256);

		convert(uncompressed, 32, compressed);

		return ClockError::SUCCESS;
	}

	ClockError HuffmanFixed::decompress(const std::string & compressed, std::string & decompressed) {
		if (compressed.length() < 4) {
			return ClockError::INVALID_ARGUMENT;
		}
		std::string realText(compressed.begin() + 4, compressed.end());

		size_t length = size_t(uint8_t(compressed[0]) * 256 * 256 * 256 + uint8_t(compressed[1]) * 256 * 256 + uint8_t(compressed[2]) * 256 + uint8_t(compressed[3]));

		if (length > INT32_MAX / 2) {
			return ClockError::INVALID_ARGUMENT;
		}

		decompressed = std::string(length, 0x0);

		return getChar(realText, tree, length, decompressed);
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
