#include <cstdint>

#include "clockUtils/errors.h"
#include "clockUtils/compression/Compression.h"
#include "clockUtils/compression/algorithm/HuffmanGeneric.h"

#include "gtest/gtest.h"

TEST(Compression, HuffmanGeneric) {
	std::string before = "Hallo Welt!";

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> compressor(before);

	std::string compressed = compressor.compress();

	clockUtils::compression::Compression<clockUtils::compression::algorithm::HuffmanGeneric> decompressor(compressed);

	std::string after = decompressor.decompress();

	EXPECT_NE(before, compressed);
	EXPECT_NE(compressed, after);
	EXPECT_EQ(before, after);
}
