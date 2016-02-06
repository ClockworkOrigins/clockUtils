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

#include <map>

#include "clockUtils/errors.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	ClockError HuffmanGeneric::compress(const std::string & uncompressed, std::string & compressed) {
		if (uncompressed.length() > INT32_MAX / 2) {
			return ClockError::INVALID_ARGUMENT;
		}
		std::vector<uint8_t> header = getHeader(uncompressed);

		compressed = std::string(header.begin(), header.end());

		compressed += char((((uncompressed.length() / 256) / 256) / 256) % 256);
		compressed += char(((uncompressed.length() / 256) / 256) % 256);
		compressed += char((uncompressed.length() / 256) % 256);
		compressed += char(uncompressed.length() % 256);
		compressed += char(uint8_t(0x0));

		std::shared_ptr<Tree> tree = buildTree(header);

		convert(uncompressed, tree, 260 * 8, compressed);

		return ClockError::SUCCESS;
	}

	ClockError HuffmanGeneric::decompress(const std::string & compressed, std::string & decompressed) {
		if (compressed.length() < 260) { // length of the string + header
			return ClockError::INVALID_ARGUMENT;
		}
		std::vector<uint8_t> header(compressed.begin(), compressed.begin() + 256);

		std::shared_ptr<Tree> tree = buildTree(header);

		std::string realText(compressed.begin() + 260, compressed.end());

		size_t length = size_t(uint8_t(compressed[256])) * 256 * 256 * 256 + size_t(uint8_t(compressed[257])) * 256 * 256 + size_t(uint8_t(compressed[258])) * 256 + size_t(uint8_t(compressed[259]));

		if (length > INT32_MAX / 2) {
			return ClockError::INVALID_ARGUMENT;
		}

		decompressed = std::string(length, 0x0);

		return getChar(realText, tree, length, decompressed);
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
