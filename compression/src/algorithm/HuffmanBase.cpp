#include "clockUtils/compression/algorithm/HuffmanBase.h"

#include <iostream>
#include <queue>

namespace clockUtils {
namespace compression {
namespace algorithm {

	std::vector<unsigned char> HuffmanBase::getHeader(const std::string & text) {
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

	std::shared_ptr<HuffmanBase::Tree> HuffmanBase::buildTree(const std::vector<unsigned char> & header) {
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
			node->c = unsigned char(i);
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

	std::string HuffmanBase::getBits(unsigned char c, const std::shared_ptr<Tree> & tree) {
		std::string result = getBitsRec(c, tree, tree->left);

		if (result.empty()) {
			result = getBitsRec(c, tree, tree->right);
		}

		return result;
	}

	std::string HuffmanBase::getBitsRec(unsigned char c, const std::shared_ptr<Tree> & tree, const std::shared_ptr<Node> & node) {
		if (node->left == nullptr && node->right == nullptr) {
			if (node->c == c) {
				if (node->parent != nullptr) {
					return (node->parent->left == node) ? "1" : "0";
				} else {
					return (tree->left == node) ? "1" : "0";
				}
			} else {
				return "";
			}
		}
		std::string result;
		
		if (node->left != nullptr) {
			result = getBitsRec(c, tree, node->left);
			if (!result.empty()) {
				if (node->parent != nullptr) {
					result = ((node->parent->left == node) ? "1" : "0") + result;
				} else {
					result = ((tree->left == node) ? "1" : "0") + result;
				}
				return result;
			}
		}

		if (node->right != nullptr) {
			result = getBitsRec(c, tree, node->right);
			if (!result.empty()) {
				if (node->parent != nullptr) {
					result = ((node->parent->left == node) ? "1" : "0") + result;
				} else {
					result = ((tree->left == node) ? "1" : "0") + result;
				}
				return result;
			}
		}

		return result;
	}

	void HuffmanBase::getChar(const std::string & compressed, const std::shared_ptr<HuffmanBase::Tree> & tree, size_t length, std::string & result) {
		// start char
		size_t index = 0;
		unsigned char currentChar = compressed[index];
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

	unsigned char HuffmanBase::convertToChar(const std::string & code) {
		unsigned char result = 0x0;

		int tmp = 0;

		if (code[0] == '1') {
			tmp += 128;
		}
		if (code.length() > 1 && code[1] == '1') {
			tmp += 64;
		}
		if (code.length() > 2 && code[2] == '1') {
			tmp += 32;
		}
		if (code.length() > 3 && code[3] == '1') {
			tmp += 16;
		}
		if (code.length() > 4 && code[4] == '1') {
			tmp += 8;
		}
		if (code.length() > 5 && code[5] == '1') {
			tmp += 4;
		}
		if (code.length() > 6 && code[6] == '1') {
			tmp += 2;
		}
		if (code.length() > 7 && code[7] == '1') {
			tmp += 1;
		}

		result = tmp;

		return result;
	}

} /* namespace algorithm */
} /* namespace compression */
} /* namespace clockUtils */
