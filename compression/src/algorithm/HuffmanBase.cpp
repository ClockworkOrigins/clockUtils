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

#include "clockUtils/compression/algorithm/HuffmanBase.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	#include <cmath>
#endif
#include <queue>

namespace clockUtils {
namespace compression {
namespace algorithm {

	HuffmanBase::Tree::~Tree() {
		deleteHelper(left);
		deleteHelper(right);

		left = nullptr;
		right = nullptr;
	}

	void HuffmanBase::Tree::deleteHelper(const std::shared_ptr<HuffmanBase::Node> & node) {
		if (node == nullptr) {
			return;
		}
		deleteHelper(node->left);
		deleteHelper(node->right);
		node->left = nullptr;
		node->right = nullptr;
		node->parent = nullptr;
	}

	std::shared_ptr<HuffmanBase::Tree> HuffmanBase::buildTree(const std::vector<uint8_t> & header) {
		std::shared_ptr<Tree> tree = std::make_shared<Tree>();

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

		while (queue.size() > 2) {
			std::shared_ptr<Node> left = queue.top();
			queue.pop();
			std::shared_ptr<Node> right = queue.top();
			queue.pop();
			std::shared_ptr<Node> node = std::make_shared<Node>();
			node->left = left;
			node->right = right;
			node->value = left->value + right->value;
			left->parent = node;
			right->parent = node;
			queue.push(node);
		}

		if (queue.size() > 0) {
			tree->left = queue.top();
			queue.pop();
		}
		if (queue.size() > 0) {
			tree->right = queue.top();
			queue.pop();
		}

		return tree;
	}

	size_t HuffmanBase::getBitsRec(uint8_t c, const std::shared_ptr<Tree> & tree, const std::shared_ptr<Node> & node, size_t count, size_t index, std::string & result) {
		if (node->left == nullptr && node->right == nullptr) {
			if (node->c == c) {
				// add new characters if necessary
				while (index / 8 + 1 > result.size()) {
					result += char(uint8_t(0x0));
				}
				if (node->parent != nullptr) {
					result[index / 8] = uint8_t(result[index / 8]) + ((node->parent->left == node) ? uint8_t(std::pow(2, (7 - index % 8))) : 0);
				} else {
					result[index / 8] = uint8_t(result[index / 8]) + ((tree->left == node) ? uint8_t(std::pow(2, (7 - index % 8))) : 0);
				}
				return count;
			} else {
				return 0;
			}
		}
		if (node->left != nullptr) {
			size_t foundCount = getBitsRec(c, tree, node->left, count + 1, index + 1, result);
			if (foundCount > 0) {
				if (node->parent != nullptr) {
					result[index / 8] = uint8_t(result[index / 8]) + ((node->parent->left == node) ? uint8_t(std::pow(2, (7 - index % 8))) : 0);
				} else {
					result[index / 8] = uint8_t(result[index / 8]) + ((tree->left == node) ? uint8_t(std::pow(2, (7 - index % 8))) : 0);
				}
				return foundCount;
			}
		}

		if (node->right != nullptr) {
			size_t foundCount = getBitsRec(c, tree, node->right, count + 1, index + 1, result);
			if (foundCount > 0) {
				if (node->parent != nullptr) {
					result[index / 8] = uint8_t(result[index / 8]) + ((node->parent->left == node) ? uint8_t(std::pow(2, (7 - index % 8))) : 0);
				} else {
					result[index / 8] = uint8_t(result[index / 8]) + ((tree->left == node) ? uint8_t(std::pow(2, (7 - index % 8))) : 0);
				}
				return foundCount;
			}
		}

		return 0;
	}

	void HuffmanBase::getChar(const std::string & compressed, const std::shared_ptr<HuffmanBase::Tree> & tree, size_t length, std::string & result) {
		// start char
		size_t index = 0;
		uint8_t currentChar = compressed[index];
		// get single bit
		for (size_t i = 0; i < length; i++) {
			std::shared_ptr<Node> node = ((currentChar & (1 << (7 - index % 8))) == (1 << (7 - index % 8))) ? tree->left : tree->right;
			index++;
			if (index % 8 == 0) {
				currentChar = compressed[index / 8];
			}
			while (node->left != nullptr || node->right != nullptr) {
				node = ((currentChar & (1 << (7 - index % 8))) == (1 << (7 - index % 8))) ? node->left : node->right;
				index++;
				if (index % 8 == 0) {
					currentChar = compressed[index / 8];
				}
			}
			result[i] = node->c;
		}
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
