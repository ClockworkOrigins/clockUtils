#include <cstdint>

#include "clockUtils/errors.h"
#include "clockUtils/argparser/ArgumentParser.h"

#include "gtest/gtest.h"

TEST(ArgumentParser, parseBool) {
	REGISTER_VARIABLE(bool, b, false, "A test boolean");
	REGISTER_VARIABLE(bool, d, false, "A test boolean");
	REGISTER_VARIABLE(bool, foo, false, "A test boolean");
	REGISTER_VARIABLE(bool, bar, false, "A test boolean");

	char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	char * buffer2[] = { "-b" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	char * buffer3[] = { "-b", "-d", "-foo", "-bar" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	EXPECT_EQ(false, b);

	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, PARSE_ARGUMENTS(buffer1, length1));

	EXPECT_EQ("argument -c not registered!", GETLASTPARSERERROR());
	EXPECT_EQ(false, b);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer2, length2));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(true, b);

	b = false;

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer3, length3));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(true, b);
	EXPECT_EQ(true, d);
	EXPECT_EQ(true, foo);
	EXPECT_EQ(true, bar);
}

TEST(ArgumentParser, parseString) {
	REGISTER_VARIABLE(std::string, s, "test", "A test string");

	char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	char * buffer2[] = { "-s" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	char * buffer3[] = { "-s", "blafoo" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	char * buffer4[] = { "-sblafoo" };
	int length4 = sizeof(buffer4) / sizeof(char *);

	char * buffer5[] = { "-s=blafoo" };
	int length5 = sizeof(buffer5) / sizeof(char *);

	EXPECT_EQ("test", s);

	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, PARSE_ARGUMENTS(buffer1, length1));

	EXPECT_FALSE(GETLASTPARSERERROR().empty());
	EXPECT_EQ("argument -c not registered!", GETLASTPARSERERROR());

	EXPECT_EQ("test", s);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer2, length2));

	EXPECT_FALSE(GETLASTPARSERERROR().empty());

	EXPECT_EQ("test", s);
	EXPECT_EQ("s requires a value: -s <value> or -s<value> or -s=<value>", GETLASTPARSERERROR());

	s = "";

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer3, length3));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ("blafoo", s);

	s = "";

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer4, length4));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ("blafoo", s);

	s = "";

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer5, length5));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ("blafoo", s);
}

TEST(ArgumentParser, parseInt) {
	REGISTER_VARIABLE(int32_t, i, -1, "A test integer");

	char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	char * buffer2[] = { "-i" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	char * buffer3[] = { "-i", "blafoo" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	char * buffer4[] = { "-i", "10" };
	int length4 = sizeof(buffer4) / sizeof(char *);

	char * buffer5[] = { "-i11" };
	int length5 = sizeof(buffer5) / sizeof(char *);

	char * buffer6[] = { "-i=12" };
	int length6 = sizeof(buffer6) / sizeof(char *);

	EXPECT_EQ(-1, i);

	EXPECT_EQ(clockUtils::ClockError::INVALID_ARGUMENT, PARSE_ARGUMENTS(buffer1, length1));

	EXPECT_FALSE(GETLASTPARSERERROR().empty());
	EXPECT_EQ("argument -c not registered!", GETLASTPARSERERROR());

	EXPECT_EQ(-1, i);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer2, length2));

	EXPECT_FALSE(GETLASTPARSERERROR().empty());
	EXPECT_EQ("i requires a value: -i <value> or -i<value> or -i=<value>", GETLASTPARSERERROR());

	EXPECT_EQ(-1, i);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer3, length3));

	EXPECT_FALSE(GETLASTPARSERERROR().empty());
	EXPECT_EQ("blafoo is not a valid value for variable i", GETLASTPARSERERROR());

	i = -1;

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer4, length4));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(10, i);

	i = -1;

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer5, length5));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(11, i);

	i = -1;

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer6, length6));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(12, i);
}

TEST(ArgumentParser, parseMultiple) {
	REGISTER_VARIABLE(int32_t, i, -1, "A test integer");
	REGISTER_VARIABLE(std::string, s, "empty", "A test string");
	REGISTER_VARIABLE(bool, b, false, "A test bool");
	REGISTER_VARIABLE(double, d, 1.23, "A test double");

	char * buffer1[] = { "-i", "1234", "-s", "readString", "-b", "-d=3.14" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer1, length1));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(1234, i);
	EXPECT_EQ("readString", s);
	EXPECT_EQ(true, b);
	EXPECT_DOUBLE_EQ(3.14, d);
}
