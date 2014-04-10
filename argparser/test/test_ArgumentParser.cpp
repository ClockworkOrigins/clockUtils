#include <cstdint>

#include "clockUtils/argparser/ArgumentParser.h"

#include "gtest/gtest.h"

TEST(ArgumentParser, parseBool) {
	REGISTER_VARIABLE(bool, b, false, "A test boolean");

	char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	char * buffer2[] = { "-b" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	std::stringstream buffer;
	std::streambuf * sbuf = std::cerr.rdbuf();
	std::cerr.rdbuf(buffer.rdbuf());

	EXPECT_EQ(false, b);

	PARSE_ARGUMENTS(buffer1, length1);

	EXPECT_TRUE(buffer.str().empty());
	EXPECT_EQ(false, b);

	PARSE_ARGUMENTS(buffer2, length2);

	EXPECT_TRUE(buffer.str().empty());

	EXPECT_EQ(true, b);

	std::cerr.rdbuf(sbuf);
}

TEST(ArgumentParser, parseString) {
	REGISTER_VARIABLE(std::string, s, "test", "A test string");

	char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	char * buffer2[] = { "-s" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	char * buffer3[] = { "-s", "blafoo" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	std::stringstream buffer;
	std::streambuf * sbuf = std::cerr.rdbuf();
	std::cerr.rdbuf(buffer.rdbuf());

	EXPECT_EQ("test", s);

	PARSE_ARGUMENTS(buffer1, length1);

	EXPECT_TRUE(buffer.str().empty());

	EXPECT_EQ("test", s);

	PARSE_ARGUMENTS(buffer2, length2);

	EXPECT_FALSE(buffer.str().empty());

	EXPECT_EQ("test", s);
	EXPECT_EQ("s requires a value: -s <value>\n", buffer.str());
	buffer.str("");

	PARSE_ARGUMENTS(buffer3, length3);

	EXPECT_TRUE(buffer.str().empty());

	EXPECT_EQ("blafoo", s);

	std::cerr.rdbuf(sbuf);
}

TEST(ArgumentParser, parseInt) {
	REGISTER_VARIABLE(int32_t, i, -1, "A test integer");

	char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	char * buffer2[] = { "-i" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	char * buffer3[] = { "-i", "blafoo" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	char * buffer4[] = { "-i", "0" };
	int length4 = sizeof(buffer4) / sizeof(char *);

	std::stringstream buffer;
	std::streambuf * sbuf = std::cerr.rdbuf();
	std::cerr.rdbuf(buffer.rdbuf());

	EXPECT_EQ(-1, i);

	PARSE_ARGUMENTS(buffer1, length1);

	EXPECT_TRUE(buffer.str().empty());
	buffer.str().clear();

	EXPECT_EQ(-1, i);

	PARSE_ARGUMENTS(buffer2, length2);

	EXPECT_FALSE(buffer.str().empty());
	EXPECT_EQ("i requires a value: -i <value>\n", buffer.str());
	buffer.str("");

	EXPECT_EQ(-1, i);

	PARSE_ARGUMENTS(buffer3, length3);

	EXPECT_FALSE(buffer.str().empty());
	EXPECT_EQ("blafoo is not a valid value for variable i\n", buffer.str());
	buffer.str("");

	EXPECT_EQ(-1, i);

	PARSE_ARGUMENTS(buffer4, length4);

	EXPECT_TRUE(buffer.str().empty());
	buffer.str("");

	EXPECT_EQ(0, i);

	std::cerr.rdbuf(sbuf);
}
