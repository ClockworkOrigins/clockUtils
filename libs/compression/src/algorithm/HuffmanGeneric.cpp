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

#include "clockUtils/compression/algorithm/HuffmanGeneric.h"

#include <limits>

#include "clockUtils/errors.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	ClockError HuffmanGeneric::compress(const std::string & uncompressed, std::string & compressed) {
		if (uncompressed.length() > std::numeric_limits<len_t>::max()) {
			// string is too long
			return ClockError::INVALID_ARGUMENT;
		}
		std::vector<uint8_t> header = calcFrequencies(uncompressed);

		compressed = std::string(header.begin(), header.end());

		len_t len = uncompressed.length();
		for (size_t i = 0; i < sizeof(len_t); i++) {
			compressed += uint8_t((len >> (8 * (sizeof(len_t) - 1 - i))) & 0xFF);
		}

		std::shared_ptr<Node> root = buildTree(header);

		convert(uncompressed, root, (256 + sizeof(len_t)) * 8, compressed);

		return ClockError::SUCCESS;
	}

	ClockError HuffmanGeneric::decompress(const std::string & compressed, std::string & decompressed) {
		if (compressed.length() < (256 + sizeof(len_t))) { // length of the header
			return ClockError::INVALID_ARGUMENT;
		}
		std::vector<uint8_t> header(compressed.begin(), compressed.begin() + 256);

		std::shared_ptr<Node> root = buildTree(header);

		std::string realText(compressed.begin() + (256 + sizeof(len_t)), compressed.end());

		len_t len = 0;
		for (size_t i = 0; i < sizeof(len_t); i++) {
			len *= 0x100; // * 256
			len += uint8_t(compressed[256 + i]);
		}

		try {
			decompressed = std::string(len, 0x0);
		} catch (std::bad_alloc & ba) {
			return ClockError::OUT_OF_MEMORY;
		}

		return getChar(realText, root, len, decompressed);
	}

	void HuffmanGeneric::convert(const std::string & text, const std::shared_ptr<Node> & root, size_t index, std::string & result) {
		std::vector<std::vector<bool>> mappings(256);
		generateMapping(root, std::vector<bool>(), mappings);

		for (uint8_t c : text) {
			size_t len = mappings[c].size();

			while ((index + len) / 8 + 1 > result.size()) {
				result += char(uint8_t(0x0));
			}

			for (size_t i = 0; i < len; i++) {
				result[(index + i) / 8] |= mappings[c][i] ? (1 << (7 - (index + i) % 8)) : 0;
			}

			index += len;
		}
	}

	std::vector<uint8_t> HuffmanGeneric::calcFrequencies(const std::string & text) {
		std::vector<len_t> header(256, 0);

		len_t max = 0;

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
