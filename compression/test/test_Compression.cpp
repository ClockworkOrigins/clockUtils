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

#include <random>

#include "clockUtils/errors.h"

#include "clockUtils/compression/Compression.h"
#include "clockUtils/compression/algorithm/HuffmanFixed.h"
#include "clockUtils/compression/algorithm/HuffmanGeneric.h"

#include "gtest/gtest.h"

TEST(Compression, HuffmanFixed) {
	std::string before("Hallo Welt!");
	std::string beforeLong("The technique works by creating a binary tree of nodes. These can be stored in a regular array, the size of which depends on the number of symbols, n. A node can be either a leaf node or an internal node. Initially, all nodes are leaf nodes, which contain the symbol itself, the weight (frequency of appearance) of the symbol and optionally, a link to a parent node which makes it easy to read the code (in reverse) starting from a leaf node. Internal nodes contain symbol weight, links to two child nodes and the optional link to a parent node. As a common convention, bit '0' represents following the left child and bit '1' represents following the right child. A finished tree has up to n leaf nodes and n-1 internal nodes. A Huffman tree that omits unused symbols produces the most optimal code lengths.\nThe process essentially begins with the leaf nodes containing the probabilities of the symbol they represent, then a new node whose children are the 2 nodes with smallest probability is created, such that the new node's probability is equal to the sum of the children's probability.With the previous 2 nodes merged into one node(thus not considering them anymore), and with the new node being now considered, the procedure is repeated until only one node remains, the Huffman tree.\nThe simplest construction algorithm uses a priority queue where the node with lowest probability is given highest priority :\nCreate a leaf node for each symbol and add it to the priority queue.\nWhile there is more than one node in the queue :\nRemove the two nodes of highest priority(lowest probability) from the queue\nCreate a new internal node with these two nodes as children and with probability equal to the sum of the two nodes' probabilities.\nAdd the new node to the queue.\nThe remaining node is the root node and the tree is complete.\nSince efficient priority queue data structures require O(log n) time per insertion, and a tree with n leaves has 2n?1 nodes, this algorithm operates in O(n log n) time, where n is the number of symbols.\nIf the symbols are sorted by probability, there is a linear - time(O(n)) method to create a Huffman tree using two queues, the first one containing the initial weights(along with pointers to the associated leaves), and combined weights(along with pointers to the trees) being put in the back of the second queue.This assures that the lowest weight is always kept at the front of one of the two queues :\nStart with as many leaves as there are symbols.\nEnqueue all leaf nodes into the first queue(by probability in increasing order so that the least likely item is in the head of the queue).\nWhile there is more than one node in the queues :\nDequeue the two nodes with the lowest weight by examining the fronts of both queues.\nCreate a new internal node, with the two just - removed nodes as children(either node can be either child) and the sum of their weights as the new weight.\nEnqueue the new node into the rear of the second queue.\nThe remaining node is the root node; the tree has now been generated.\nAlthough linear - time given sorted input, in the general case of arbitrary input, using this algorithm requires pre - sorting.Thus, since sorting takes O(n log n) time in the general case, both methods have the same overall complexity.\nIn many cases, time complexity is not very important in the choice of algorithm here, since n here is the number of symbols in the alphabet, which is typically a very small number(compared to the length of the message to be encoded); whereas complexity analysis concerns the behavior when n grows to be very large.\nIt is generally beneficial to minimize the variance of codeword length.For example, a communication buffer receiving Huffman - encoded data may need to be larger to deal with especially long symbols if the tree is especially unbalanced.To minimize variance, simply break ties between queues by choosing the item in the first queue.This modification will retain the mathematical optimality of the Huffman coding while both minimizing variance and minimizing the length of the longest character code.\nHere's an example of optimized Huffman coding using the French subject string \"j'aime aller sur le bord de l'eau les jeudis ou les jours impairs\". Note that original Huffman coding tree structure would be different from the given example");
	std::string beforeVeryLong;

	std::vector<unsigned char> allTheSameVec(10000, 'a');
	std::string beforeAllTheSame(allTheSameVec.begin(), allTheSameVec.end());

	std::default_random_engine generator;
	std::binomial_distribution<int> distribution(255, 0.5);

	for (unsigned int i = 0; i < 50000; i++) {
		beforeVeryLong += char(distribution(generator));
	}

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanFixed> compressor;

	std::string compressed;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(before, compressed));
	std::string compressedLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(beforeLong, compressedLong));
	std::string compressedVeryLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(beforeVeryLong, compressedVeryLong));
	std::string compressedAllTheSame;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(beforeAllTheSame, compressedAllTheSame));

	std::string after;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressed, after));
	std::string afterLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressedLong, afterLong));
	std::string afterVeryLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressedVeryLong, afterVeryLong));
	std::string afterAllTheSame;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressedAllTheSame, afterAllTheSame));

	EXPECT_NE(before, compressed);
	EXPECT_NE(compressed, after);
	EXPECT_EQ(before, after);

	EXPECT_NE(beforeLong, compressedLong);
	EXPECT_NE(compressedLong, afterLong);
	EXPECT_EQ(beforeLong, afterLong);

	EXPECT_LT(compressedLong.length(), beforeLong.length());

	EXPECT_NE(beforeVeryLong, compressedVeryLong);
	EXPECT_NE(compressedVeryLong, afterVeryLong);
	EXPECT_EQ(beforeVeryLong, afterVeryLong);

	EXPECT_GT(compressedVeryLong.length(), beforeVeryLong.length());

	EXPECT_NE(beforeAllTheSame, compressedAllTheSame);
	EXPECT_NE(compressedAllTheSame, afterAllTheSame);
	EXPECT_EQ(beforeAllTheSame, afterAllTheSame);

	EXPECT_LT(compressedAllTheSame.length(), beforeAllTheSame.length());
}

TEST(Compression, HuffmanGeneric) {
	std::string before("Hallo Welt!");
	std::string beforeLong("The technique works by creating a binary tree of nodes. These can be stored in a regular array, the size of which depends on the number of symbols, n. A node can be either a leaf node or an internal node. Initially, all nodes are leaf nodes, which contain the symbol itself, the weight (frequency of appearance) of the symbol and optionally, a link to a parent node which makes it easy to read the code (in reverse) starting from a leaf node. Internal nodes contain symbol weight, links to two child nodes and the optional link to a parent node. As a common convention, bit '0' represents following the left child and bit '1' represents following the right child. A finished tree has up to n leaf nodes and n-1 internal nodes. A Huffman tree that omits unused symbols produces the most optimal code lengths.\nThe process essentially begins with the leaf nodes containing the probabilities of the symbol they represent, then a new node whose children are the 2 nodes with smallest probability is created, such that the new node's probability is equal to the sum of the children's probability.With the previous 2 nodes merged into one node(thus not considering them anymore), and with the new node being now considered, the procedure is repeated until only one node remains, the Huffman tree.\nThe simplest construction algorithm uses a priority queue where the node with lowest probability is given highest priority :\nCreate a leaf node for each symbol and add it to the priority queue.\nWhile there is more than one node in the queue :\nRemove the two nodes of highest priority(lowest probability) from the queue\nCreate a new internal node with these two nodes as children and with probability equal to the sum of the two nodes' probabilities.\nAdd the new node to the queue.\nThe remaining node is the root node and the tree is complete.\nSince efficient priority queue data structures require O(log n) time per insertion, and a tree with n leaves has 2n?1 nodes, this algorithm operates in O(n log n) time, where n is the number of symbols.\nIf the symbols are sorted by probability, there is a linear - time(O(n)) method to create a Huffman tree using two queues, the first one containing the initial weights(along with pointers to the associated leaves), and combined weights(along with pointers to the trees) being put in the back of the second queue.This assures that the lowest weight is always kept at the front of one of the two queues :\nStart with as many leaves as there are symbols.\nEnqueue all leaf nodes into the first queue(by probability in increasing order so that the least likely item is in the head of the queue).\nWhile there is more than one node in the queues :\nDequeue the two nodes with the lowest weight by examining the fronts of both queues.\nCreate a new internal node, with the two just - removed nodes as children(either node can be either child) and the sum of their weights as the new weight.\nEnqueue the new node into the rear of the second queue.\nThe remaining node is the root node; the tree has now been generated.\nAlthough linear - time given sorted input, in the general case of arbitrary input, using this algorithm requires pre - sorting.Thus, since sorting takes O(n log n) time in the general case, both methods have the same overall complexity.\nIn many cases, time complexity is not very important in the choice of algorithm here, since n here is the number of symbols in the alphabet, which is typically a very small number(compared to the length of the message to be encoded); whereas complexity analysis concerns the behavior when n grows to be very large.\nIt is generally beneficial to minimize the variance of codeword length.For example, a communication buffer receiving Huffman - encoded data may need to be larger to deal with especially long symbols if the tree is especially unbalanced.To minimize variance, simply break ties between queues by choosing the item in the first queue.This modification will retain the mathematical optimality of the Huffman coding while both minimizing variance and minimizing the length of the longest character code.\nHere's an example of optimized Huffman coding using the French subject string \"j'aime aller sur le bord de l'eau les jeudis ou les jours impairs\". Note that original Huffman coding tree structure would be different from the given example");
	std::string beforeVeryLong;

	std::vector<unsigned char> allTheSameVec(10000, 'a');
	std::string beforeAllTheSame(allTheSameVec.begin(), allTheSameVec.end());

	std::default_random_engine generator;
	std::binomial_distribution<int> distribution(255, 0.5);

	for (unsigned int i = 0; i < 50000; i++) {
		beforeVeryLong += char(distribution(generator));
	}

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> compressor;

	std::string compressed;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(before, compressed));
	std::string compressedLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(beforeLong, compressedLong));
	std::string compressedVeryLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(beforeVeryLong, compressedVeryLong));
	std::string compressedAllTheSame;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.compress(beforeAllTheSame, compressedAllTheSame));

	std::string after;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressed, after));
	std::string afterLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressedLong, afterLong));
	std::string afterVeryLong;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressedVeryLong, afterVeryLong));
	std::string afterAllTheSame;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressor.decompress(compressedAllTheSame, afterAllTheSame));

	EXPECT_NE(before, compressed);
	EXPECT_NE(compressed, after);
	EXPECT_EQ(before, after);

	EXPECT_NE(beforeLong, compressedLong);
	EXPECT_NE(compressedLong, afterLong);
	EXPECT_EQ(beforeLong, afterLong);

	EXPECT_LT(compressedLong.length(), beforeLong.length());

	EXPECT_NE(beforeVeryLong, compressedVeryLong);
	EXPECT_NE(compressedVeryLong, afterVeryLong);
	EXPECT_EQ(beforeVeryLong, afterVeryLong);

	EXPECT_LT(compressedVeryLong.length(), beforeVeryLong.length());

	EXPECT_NE(beforeAllTheSame, compressedAllTheSame);
	EXPECT_NE(compressedAllTheSame, afterAllTheSame);
	EXPECT_EQ(beforeAllTheSame, afterAllTheSame);

	EXPECT_LT(compressedAllTheSame.length(), beforeAllTheSame.length());
}

TEST(Compression, HuffmanComparison) {
	std::string before("Hallo Welt!");
	std::string beforeLong("The technique works by creating a binary tree of nodes. These can be stored in a regular array, the size of which depends on the number of symbols, n. A node can be either a leaf node or an internal node. Initially, all nodes are leaf nodes, which contain the symbol itself, the weight (frequency of appearance) of the symbol and optionally, a link to a parent node which makes it easy to read the code (in reverse) starting from a leaf node. Internal nodes contain symbol weight, links to two child nodes and the optional link to a parent node. As a common convention, bit '0' represents following the left child and bit '1' represents following the right child. A finished tree has up to n leaf nodes and n-1 internal nodes. A Huffman tree that omits unused symbols produces the most optimal code lengths.\nThe process essentially begins with the leaf nodes containing the probabilities of the symbol they represent, then a new node whose children are the 2 nodes with smallest probability is created, such that the new node's probability is equal to the sum of the children's probability.With the previous 2 nodes merged into one node(thus not considering them anymore), and with the new node being now considered, the procedure is repeated until only one node remains, the Huffman tree.\nThe simplest construction algorithm uses a priority queue where the node with lowest probability is given highest priority :\nCreate a leaf node for each symbol and add it to the priority queue.\nWhile there is more than one node in the queue :\nRemove the two nodes of highest priority(lowest probability) from the queue\nCreate a new internal node with these two nodes as children and with probability equal to the sum of the two nodes' probabilities.\nAdd the new node to the queue.\nThe remaining node is the root node and the tree is complete.\nSince efficient priority queue data structures require O(log n) time per insertion, and a tree with n leaves has 2n?1 nodes, this algorithm operates in O(n log n) time, where n is the number of symbols.\nIf the symbols are sorted by probability, there is a linear - time(O(n)) method to create a Huffman tree using two queues, the first one containing the initial weights(along with pointers to the associated leaves), and combined weights(along with pointers to the trees) being put in the back of the second queue.This assures that the lowest weight is always kept at the front of one of the two queues :\nStart with as many leaves as there are symbols.\nEnqueue all leaf nodes into the first queue(by probability in increasing order so that the least likely item is in the head of the queue).\nWhile there is more than one node in the queues :\nDequeue the two nodes with the lowest weight by examining the fronts of both queues.\nCreate a new internal node, with the two just - removed nodes as children(either node can be either child) and the sum of their weights as the new weight.\nEnqueue the new node into the rear of the second queue.\nThe remaining node is the root node; the tree has now been generated.\nAlthough linear - time given sorted input, in the general case of arbitrary input, using this algorithm requires pre - sorting.Thus, since sorting takes O(n log n) time in the general case, both methods have the same overall complexity.\nIn many cases, time complexity is not very important in the choice of algorithm here, since n here is the number of symbols in the alphabet, which is typically a very small number(compared to the length of the message to be encoded); whereas complexity analysis concerns the behavior when n grows to be very large.\nIt is generally beneficial to minimize the variance of codeword length.For example, a communication buffer receiving Huffman - encoded data may need to be larger to deal with especially long symbols if the tree is especially unbalanced.To minimize variance, simply break ties between queues by choosing the item in the first queue.This modification will retain the mathematical optimality of the Huffman coding while both minimizing variance and minimizing the length of the longest character code.\nHere's an example of optimized Huffman coding using the French subject string \"j'aime aller sur le bord de l'eau les jeudis ou les jours impairs\". Note that original Huffman coding tree structure would be different from the given example");

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> compressorGeneric;
	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanFixed> compressorFixed;

	std::string compressedGeneric;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorGeneric.compress(before, compressedGeneric));
	std::string compressedLongGeneric;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorGeneric.compress(beforeLong, compressedLongGeneric));

	std::string compressedFixed;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorFixed.compress(before, compressedFixed));
	std::string compressedLongFixed;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorFixed.compress(beforeLong, compressedLongFixed));

	EXPECT_LT(compressedFixed.length(), compressedGeneric.length());
	EXPECT_LT(compressedLongFixed.length(), compressedLongGeneric.length());
}

TEST(Compression, decompressUncompressedString) {
	std::string before("Hallo Welt!");
	std::string beforeLong("The technique works by creating a binary tree of nodes. These can be stored in a regular array, the size of which depends on the number of symbols, n. A node can be either a leaf node or an internal node. Initially, all nodes are leaf nodes, which contain the symbol itself, the weight (frequency of appearance) of the symbol and optionally, a link to a parent node which makes it easy to read the code (in reverse) starting from a leaf node. Internal nodes contain symbol weight, links to two child nodes and the optional link to a parent node. As a common convention, bit '0' represents following the left child and bit '1' represents following the right child. A finished tree has up to n leaf nodes and n-1 internal nodes. A Huffman tree that omits unused symbols produces the most optimal code lengths.\nThe process essentially begins with the leaf nodes containing the probabilities of the symbol they represent, then a new node whose children are the 2 nodes with smallest probability is created, such that the new node's probability is equal to the sum of the children's probability.With the previous 2 nodes merged into one node(thus not considering them anymore), and with the new node being now considered, the procedure is repeated until only one node remains, the Huffman tree.\nThe simplest construction algorithm uses a priority queue where the node with lowest probability is given highest priority :\nCreate a leaf node for each symbol and add it to the priority queue.\nWhile there is more than one node in the queue :\nRemove the two nodes of highest priority(lowest probability) from the queue\nCreate a new internal node with these two nodes as children and with probability equal to the sum of the two nodes' probabilities.\nAdd the new node to the queue.\nThe remaining node is the root node and the tree is complete.\nSince efficient priority queue data structures require O(log n) time per insertion, and a tree with n leaves has 2n?1 nodes, this algorithm operates in O(n log n) time, where n is the number of symbols.\nIf the symbols are sorted by probability, there is a linear - time(O(n)) method to create a Huffman tree using two queues, the first one containing the initial weights(along with pointers to the associated leaves), and combined weights(along with pointers to the trees) being put in the back of the second queue.This assures that the lowest weight is always kept at the front of one of the two queues :\nStart with as many leaves as there are symbols.\nEnqueue all leaf nodes into the first queue(by probability in increasing order so that the least likely item is in the head of the queue).\nWhile there is more than one node in the queues :\nDequeue the two nodes with the lowest weight by examining the fronts of both queues.\nCreate a new internal node, with the two just - removed nodes as children(either node can be either child) and the sum of their weights as the new weight.\nEnqueue the new node into the rear of the second queue.\nThe remaining node is the root node; the tree has now been generated.\nAlthough linear - time given sorted input, in the general case of arbitrary input, using this algorithm requires pre - sorting.Thus, since sorting takes O(n log n) time in the general case, both methods have the same overall complexity.\nIn many cases, time complexity is not very important in the choice of algorithm here, since n here is the number of symbols in the alphabet, which is typically a very small number(compared to the length of the message to be encoded); whereas complexity analysis concerns the behavior when n grows to be very large.\nIt is generally beneficial to minimize the variance of codeword length.For example, a communication buffer receiving Huffman - encoded data may need to be larger to deal with especially long symbols if the tree is especially unbalanced.To minimize variance, simply break ties between queues by choosing the item in the first queue.This modification will retain the mathematical optimality of the Huffman coding while both minimizing variance and minimizing the length of the longest character code.\nHere's an example of optimized Huffman coding using the French subject string \"j'aime aller sur le bord de l'eau les jeudis ou les jours impairs\". Note that original Huffman coding tree structure would be different from the given example");

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> compressorGeneric;
	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanFixed> compressorFixed;

	std::string decompressedEmpty;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorGeneric.decompress("", decompressedEmpty));
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorFixed.decompress("", decompressedEmpty));

	std::string decompressedGeneric;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorGeneric.decompress(before, decompressedGeneric));
	std::string decompressedLongGeneric;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorGeneric.decompress(beforeLong, decompressedLongGeneric));

	std::string decompressedFixed;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorFixed.decompress(before, decompressedFixed));
	std::string decompressedLongFixed;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorFixed.decompress(beforeLong, decompressedLongFixed));
}

TEST(Compression, decompressCompressedStringWithDifferentAlgorithm) {
	std::string before("Hallo Welt!");
	std::string beforeLong("The technique works by creating a binary tree of nodes. These can be stored in a regular array, the size of which depends on the number of symbols, n. A node can be either a leaf node or an internal node. Initially, all nodes are leaf nodes, which contain the symbol itself, the weight (frequency of appearance) of the symbol and optionally, a link to a parent node which makes it easy to read the code (in reverse) starting from a leaf node. Internal nodes contain symbol weight, links to two child nodes and the optional link to a parent node. As a common convention, bit '0' represents following the left child and bit '1' represents following the right child. A finished tree has up to n leaf nodes and n-1 internal nodes. A Huffman tree that omits unused symbols produces the most optimal code lengths.\nThe process essentially begins with the leaf nodes containing the probabilities of the symbol they represent, then a new node whose children are the 2 nodes with smallest probability is created, such that the new node's probability is equal to the sum of the children's probability.With the previous 2 nodes merged into one node(thus not considering them anymore), and with the new node being now considered, the procedure is repeated until only one node remains, the Huffman tree.\nThe simplest construction algorithm uses a priority queue where the node with lowest probability is given highest priority :\nCreate a leaf node for each symbol and add it to the priority queue.\nWhile there is more than one node in the queue :\nRemove the two nodes of highest priority(lowest probability) from the queue\nCreate a new internal node with these two nodes as children and with probability equal to the sum of the two nodes' probabilities.\nAdd the new node to the queue.\nThe remaining node is the root node and the tree is complete.\nSince efficient priority queue data structures require O(log n) time per insertion, and a tree with n leaves has 2n?1 nodes, this algorithm operates in O(n log n) time, where n is the number of symbols.\nIf the symbols are sorted by probability, there is a linear - time(O(n)) method to create a Huffman tree using two queues, the first one containing the initial weights(along with pointers to the associated leaves), and combined weights(along with pointers to the trees) being put in the back of the second queue.This assures that the lowest weight is always kept at the front of one of the two queues :\nStart with as many leaves as there are symbols.\nEnqueue all leaf nodes into the first queue(by probability in increasing order so that the least likely item is in the head of the queue).\nWhile there is more than one node in the queues :\nDequeue the two nodes with the lowest weight by examining the fronts of both queues.\nCreate a new internal node, with the two just - removed nodes as children(either node can be either child) and the sum of their weights as the new weight.\nEnqueue the new node into the rear of the second queue.\nThe remaining node is the root node; the tree has now been generated.\nAlthough linear - time given sorted input, in the general case of arbitrary input, using this algorithm requires pre - sorting.Thus, since sorting takes O(n log n) time in the general case, both methods have the same overall complexity.\nIn many cases, time complexity is not very important in the choice of algorithm here, since n here is the number of symbols in the alphabet, which is typically a very small number(compared to the length of the message to be encoded); whereas complexity analysis concerns the behavior when n grows to be very large.\nIt is generally beneficial to minimize the variance of codeword length.For example, a communication buffer receiving Huffman - encoded data may need to be larger to deal with especially long symbols if the tree is especially unbalanced.To minimize variance, simply break ties between queues by choosing the item in the first queue.This modification will retain the mathematical optimality of the Huffman coding while both minimizing variance and minimizing the length of the longest character code.\nHere's an example of optimized Huffman coding using the French subject string \"j'aime aller sur le bord de l'eau les jeudis ou les jours impairs\". Note that original Huffman coding tree structure would be different from the given example");

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> compressorGeneric;
	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanFixed> compressorFixed;

	std::string compressedGeneric;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorGeneric.compress(before, compressedGeneric));
	std::string compressedLongGeneric;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorGeneric.compress(beforeLong, compressedLongGeneric));

	std::string compressedFixed;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorFixed.compress(before, compressedFixed));
	std::string compressedLongFixed;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorFixed.compress(beforeLong, compressedLongFixed));

	std::string decompressedGeneric;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorGeneric.decompress(compressedFixed, decompressedGeneric));
	std::string decompressedLongGeneric;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorGeneric.decompress(compressedLongFixed, decompressedLongGeneric));

	std::string decompressedFixed;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorFixed.decompress(compressedGeneric, decompressedFixed));
	std::string decompressedLongFixed;
	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, compressorFixed.decompress(compressedLongGeneric, decompressedLongFixed));
}

// decompress a string with exakt match of bits in bytes (bits % 8 == 0)
TEST(Compression, decompressStringWithBitByteMatch) {
	std::string s("abababab");
	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> compressorGeneric;

	std::string compressedGeneric;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorGeneric.compress(s, compressedGeneric));

	std::string decompressedGeneric;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, compressorGeneric.decompress(compressedGeneric, decompressedGeneric));

	EXPECT_EQ(decompressedGeneric, s);
}
