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

#include <cassert>
#include <limits>

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

	std::shared_ptr<HuffmanBase::Node> HuffmanFixed::root = HuffmanFixed::buildTree(preVec);
	std::vector<std::vector<bool>> HuffmanFixed::mappings = std::vector<std::vector<bool>>(256);

	ClockError HuffmanFixed::compress(const std::string & uncompressed, std::string & compressed) {
		if (uncompressed.length() > std::numeric_limits<len_t>::max()) {
			// string is too long
			return ClockError::INVALID_ARGUMENT;
		}
		compressed = std::string(sizeof(len_t), 0x0);

		len_t len = uncompressed.length();
		for (size_t i = 0; i < sizeof(len_t); i++) {
			compressed[i] = uint8_t((len >> (8 * (sizeof(len_t) - 1 - i))) & 0xFF);
		}

		convert(uncompressed, sizeof(len_t) * 8, compressed);

		return ClockError::SUCCESS;
	}

	ClockError HuffmanFixed::decompress(const std::string & compressed, std::string & decompressed) {
		if (compressed.length() < sizeof(len_t)) {
			return ClockError::INVALID_ARGUMENT;
		}
		std::string realText(compressed.begin() + sizeof(len_t), compressed.end());

		len_t len = 0;
		for (size_t i = 0; i < sizeof(len_t); i++) {
			len *= 0x100; // * 256
			len += uint8_t(compressed[i]);
		}

		decompressed = std::string(len, 0x0);

		return getChar(realText, root, len, decompressed);
	}

	void HuffmanFixed::convert(const std::string & text, size_t index, std::string & result) {
		if (mappings[0].size() == 0) {
			std::vector<bool> vec;
			generateMapping(root, vec, mappings);
		}
		for (uint8_t c : text) {
			size_t len = mappings[c].size();

			while ((index + len) / 8 + 1 > result.size()) { // FIXME if index+len % 8 == 0 -> has 1 byte too much
				result += char(uint8_t(0x0));
			}

			for (size_t i = 0; i < len; i++) {
				result[(index + i) / 8] |= mappings[c][i] ? (1 << (7 - (index + i) % 8)) : 0;
			}

			index += len;
		}
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
