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

#include "clockUtils/compression/algorithm/HuffmanBase.h"

#include <cassert>
#include <cmath>
#include <queue>

#include "clockUtils/errors.h"

namespace clockUtils {
namespace compression {
namespace algorithm {

	std::shared_ptr<HuffmanBase::Node> HuffmanBase::buildTree(const std::vector<uint8_t> & header) {
		struct compare {
			bool operator() (const std::shared_ptr<Node> & a, const std::shared_ptr<Node> & b) const {
				return a->value > b->value;
			}
		};

		std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, compare> queue;

		for (size_t i = 0; i < 256; i++) {
			if (header[i] == 0) {
				continue;
			}
			std::shared_ptr<Node> node = std::make_shared<Node>();
			node->value = header[i];
			node->c = uint8_t(i);
			queue.push(node);
		}

		while (queue.size() >= 2) {
			// combine the first two nodes
			std::shared_ptr<Node> left = queue.top();
			queue.pop();
			std::shared_ptr<Node> right = queue.top();
			queue.pop();
			std::shared_ptr<Node> node = std::make_shared<Node>();
			node->left = left;
			node->right = right;
			node->value = left->value + right->value;
			queue.push(node);
		}

		return queue.top();
	}

	void HuffmanBase::generateMapping(const std::shared_ptr<Node> & node, const std::vector<bool> & bitSeq, std::vector<std::vector<bool>> & mapping) {
		// reached leaves
		if (node->left == nullptr) {
			assert(node->right == nullptr);
			mapping[node->c] = bitSeq;
			return;
		}
		std::vector<bool> left = bitSeq;
		left.push_back(true);
		generateMapping(node->left, left, mapping);

		std::vector<bool> right = bitSeq;
		right.push_back(false);
		generateMapping(node->right, right, mapping);
	}

	ClockError HuffmanBase::getChar(const std::string & compressed, const std::shared_ptr<Node> & root, len_t length, std::string & result) {
		// first bit of first byte, actual bit index in stream is byteIndex * 8 + bitIndex
		uint8_t bitIndex = 0;
		len_t byteIndex = 0;

		uint8_t currentChar = compressed[0];

		bool endReached = false;
		for (len_t i = 0; i < length; i++) {
			std::shared_ptr<Node> node = root;
			while (node->left != nullptr || node->right != nullptr) {
				if (endReached) {
					return ClockError::INVALID_ARGUMENT;
				}
				node = (currentChar & (1 << (7 - bitIndex))) ? node->left : node->right;
				bitIndex++;
				if (bitIndex == 8) { // next byte
					bitIndex = 0;
					byteIndex++;
					if (byteIndex >= compressed.length()) { // no more bytes left
						endReached = true;
					} else {
						currentChar = compressed[byteIndex];
					}
				}
			}
			result[i] = node->c;
		}

		return (byteIndex == compressed.length() - 1) ? ClockError::SUCCESS : ClockError::INVALID_ARGUMENT;
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
