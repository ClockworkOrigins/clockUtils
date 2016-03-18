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

#include <cmath>
#include <cstdint>

#include "clockUtils/errors.h"
#include "clockUtils/argParser/ArgumentParser.h"

#include "gtest/gtest.h"

TEST(ArgumentParser, parseBool) {
	// (Type, long name(and variable), short name, default, desc)
	REGISTER_VARIABLE(bool, bo, b, false, "A test boolean");
	REGISTER_VARIABLE(bool, d, d, false, "A test boolean");
	REGISTER_VARIABLE(bool, foo, f, false, "A test boolean");
	REGISTER_VARIABLE(bool, bar, a, true, "A test boolean");
	REGISTER_VARIABLE(bool, longname, "", false, "A test boolean"); // no short flag

	// Check the helptext
	const char * helpText = ""
							"\t--bar, -a      [Default: true]           A test boolean\n"
							"\t--bo, -b       [Default: false]          A test boolean\n"
							"\t--d, -d        [Default: false]          A test boolean\n"
							"\t--foo, -f      [Default: false]          A test boolean\n"
							"\t--longname     [Default: false]          A test boolean";
	EXPECT_EQ(helpText, GETHELPTEXT());

	EXPECT_FALSE(bo);

	// Test invalid args
	const char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer1, length1));
	EXPECT_EQ("argument -c not registered!", GETLASTPARSERERROR());
	EXPECT_FALSE(bo);

	// Test a long flag
	const char * buffer2[] = { "--bo" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer2, length2));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());
	EXPECT_EQ(true, bo);
	EXPECT_EQ(false, d);
	EXPECT_EQ(false, foo);
	EXPECT_EQ(true, bar);
	EXPECT_EQ(false, longname);

	// Test all set long
	const char * buffer3[] = { "--bo", "--d", "--foo", "--bar" , "--longname" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer3, length3));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());
	EXPECT_EQ(true, bo);
	EXPECT_EQ(true, d);
	EXPECT_EQ(true, foo);
	EXPECT_EQ(true, bar);
	EXPECT_EQ(true, longname);

	// Test none set
	const char * buffer4[] = { "" };
	int length4 = sizeof(buffer4) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer4, length4)); // invalid because "" causes argv to be not empty, but it only contains an empty string

	EXPECT_FALSE(GETLASTPARSERERROR().empty());
	EXPECT_EQ("parsing empty string not possible!", GETLASTPARSERERROR());
	EXPECT_EQ(false, bo);
	EXPECT_EQ(false, d);
	EXPECT_EQ(false, foo);
	EXPECT_EQ(true, bar);
	EXPECT_EQ(false, longname);

	// Test all set short
	bo = false;
	const char * buffer5[] = { "-b", "-d", "-a", "-f" };
	int length5 = sizeof(buffer5) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer5, length5));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());
	EXPECT_EQ(true, bo);
	EXPECT_EQ(true, d);
	EXPECT_EQ(true, foo);
	EXPECT_EQ(true, bar);
	EXPECT_EQ(false, longname); // defaulted back

	// Set explicit
	const char * buffer6[] = { "-b", "true", "-d", "false", "--bar", "true", "--foo", "false", "--longname", "false" };
	int length6 = sizeof(buffer6) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer6, length6));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());
	EXPECT_EQ(true, bo);
	EXPECT_EQ(false, d);
	EXPECT_EQ(false, foo);
	EXPECT_EQ(true, bar);
	EXPECT_EQ(false, longname);
}

TEST(ArgumentParser, parseString) {
	REGISTER_VARIABLE(std::string, s, s, "test", "A test string");

	const char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	const char * buffer2[] = { "-s" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	const char * buffer3[] = { "-s", "blafoo" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	const char * buffer4[] = { "-s", "blafoo" };
	int length4 = sizeof(buffer4) / sizeof(char *);

	const char * buffer5[] = { "-s", "blafoo" };
	int length5 = sizeof(buffer5) / sizeof(char *);

	EXPECT_EQ("test", s);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer1, length1));

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
	REGISTER_VARIABLE(int32_t, i, i, -1, "A test integer");

	const char * buffer1[] = { "-c", "-e", "3" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	const char * buffer2[] = { "-i" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	const char * buffer3[] = { "-i", "blafoo" };
	int length3 = sizeof(buffer3) / sizeof(char *);

	const char * buffer4[] = { "-i", "10" };
	int length4 = sizeof(buffer4) / sizeof(char *);

	const char * buffer5[] = { "-i", "11" };
	int length5 = sizeof(buffer5) / sizeof(char *);

	const char * buffer6[] = { "-i", "12" };
	int length6 = sizeof(buffer6) / sizeof(char *);

	EXPECT_EQ(-1, i);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer1, length1));

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
	REGISTER_VARIABLE(int32_t, i, i, -1, "A test integer");
	REGISTER_VARIABLE(std::string, s, s, "empty", "A test string");
	REGISTER_VARIABLE(bool, b, b, false, "A test bool");
	REGISTER_VARIABLE(double, d, d, 1.23, "A test double");

	const char * buffer1[] = { "-i", "1234", "-s", "readString", "-b", "-d", "3.14" };
	int length1 = sizeof(buffer1) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer1, length1));

	EXPECT_TRUE(GETLASTPARSERERROR().empty());

	EXPECT_EQ(1234, i);
	EXPECT_EQ("readString", s);
	EXPECT_EQ(true, b);
	EXPECT_DOUBLE_EQ(3.14, d);
}

TEST(ArgumentParser, testForTrailingArgs) {
	REGISTER_VARIABLE(int32_t, i, i, -1, "A test integer");
	{
		REGISTER_VARIABLE_ARGUMENTS(args);

		const char * buffer1[] = { "-i", "1234", "a1", "a2" };
		int length1 = sizeof(buffer1) / sizeof(char *);
		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer1, length1));
		EXPECT_EQ(2, args.size());	// only two arguments remaining
		EXPECT_EQ("a1", args[0]);
		EXPECT_EQ("a2", args[1]);
	}

	{
		REGISTER_VARIABLE_ARGUMENTS(args);
		const char * buffer2[] = { "-i", "1234" };
		int length2 = sizeof(buffer2) / sizeof(char *);
		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer2, length2)); // no args trailing
		EXPECT_EQ(0, args.size());
	}

	{
		const char * buffer3[] = {"-i", "1234", "-j"};
		int length3 = sizeof(buffer3) / sizeof(char *);
		REGISTER_VARIABLE_ARGUMENTS(args);
		clockUtils::argParser::Parser::setErrorOnFlag(false);
		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer3, length3)); // no args trailing
		EXPECT_EQ(1, args.size());
		EXPECT_EQ("-j", args[0]);
	}

	{
		const char * buffer4[] = {"-i", "1234", "-j"};
		int length4 = sizeof(buffer4) / sizeof(char *);
		REGISTER_VARIABLE_ARGUMENTS(args);
		clockUtils::argParser::Parser::setErrorOnFlag(true);
		EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer4, length4)); // no args trailing
	}
}

TEST(ArgumentParser, invalidCommands) {
	REGISTER_VARIABLE(int32_t, i, i, -1, "A test integer");
	REGISTER_VARIABLE(int32_t, j, j, -1, "A test integer");

	const char * buffer1[] = { "-i", "-j", "1234", "4567" };
	int length1 = sizeof(buffer1) / sizeof(char *);
	const char * buffer2[] = { "1234", "-i", "1234", "-j", "4567" };
	int length2 = sizeof(buffer2) / sizeof(char *);

	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer1, length1)); // one variable as arg for other
	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer2, length2)); // additional leading value
}

TEST(ArgumentParser, undefinedParam) {
	REGISTER_VARIABLE_ARGUMENTS(liste);
	const char * buffer[] = { "test" , "-x", "foo", "bar"};
	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer, 4));
}

TEST(ArgumentParser, macroTransparency) {
	REGISTER_VARIABLE(std::string, s, s, "", "Sample");
	const char * buffer[] = { "-s", "foo" };
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, 2));
	EXPECT_EQ("foo", s);
	EXPECT_EQ(s, "foo");
	EXPECT_TRUE("foo" == s);
	EXPECT_FALSE("foo" != s);
}

TEST(ArgumentParser, stringWithSpace) {
	REGISTER_VARIABLE_ARGUMENTS(liste);
	REGISTER_VARIABLE(std::string, s, s, "", "Sample");
	REGISTER_VARIABLE(std::string, f, f, "", "Sample");
	REGISTER_VARIABLE(char, c, c, ' ', "Sample");
	const char * buffer[] = { "-s", "foo bar", "-f", " a b c ", "-c", " ", " a b c " };
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, 7));
	EXPECT_EQ("foo bar", s);
	EXPECT_EQ(" a b c ", f);
	EXPECT_EQ(' ', c);
	ASSERT_EQ(1, liste.size());
	EXPECT_EQ(" a b c ", liste[0]);
}

TEST(ArgumentParser, parseChar) {
	REGISTER_VARIABLE(char, c, c, ' ', "Sample");
	REGISTER_VARIABLE(char, d, d, ' ', "Sample");
	const char * buffer1[] = { "-c", "x" };
	const char * buffer2[] = { "-d", "xy" };
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer1, 2));
	EXPECT_EQ('x', c);
	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer2, 2));
}

struct Vec3 {
	double _x, _y, _z;
	Vec3() : _x(), _y(), _z() {
	}
	Vec3(double x, double y, double z) : _x(x), _y(y), _z(z) {
	}
	bool operator==(const Vec3 & other) const {
		return std::fabs(_x - other._x) < DBL_EPSILON && std::fabs(_y - other._y) < DBL_EPSILON && std::fabs(_z - other._z) < DBL_EPSILON;
	}
	friend std::istream & operator>>(std::istream & in, Vec3 & vec) {
		in >> vec._x >> vec._y >> vec._z;
		return in;
	}
	friend std::ostream & operator<<(std::ostream & out, Vec3 & vec) {
		out << vec._x << " " << vec._y << " " << vec._z;
		return out;
	}
};

TEST(ArgumentParser, parseUserDefined) {
	REGISTER_VARIABLE(Vec3, v, v, Vec3(), "Sample");
	const char * buffer1[] = { "--v", "1.0 2.0 3.0" };
	const char * buffer2[] = { "-v", "1.0 2.0 3.0 4.0" };
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer1, 2));
	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer2, 2));
	EXPECT_EQ(Vec3(1.0, 2.0, 3.0), v);
}

TEST(ArgumentParser, variableSet) {
	REGISTER_VARIABLE(std::string, s, s, "", "Sample");
	REGISTER_VARIABLE(int, i, i, 15, "Sample");
	const char * buffer[] = { "-s", "foo" };
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, 2));
	EXPECT_EQ("foo", s);
	EXPECT_EQ(15, i);
	EXPECT_TRUE(s.isSet());
	EXPECT_FALSE(i.isSet());
}

TEST(ArgumentParser, help) {
	REGISTER_VARIABLE(std::string, s, s, "", "A string variable");
	REGISTER_VARIABLE(int, i, i, 15, "An integer variable");
	REGISTER_VARIABLE(int, variable, v, 15, "A long and short variable");
	{
		const char * buffer[] = { "--help" };
		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, 1));
		EXPECT_TRUE(HELPSET());
		std::string helpText = "\t--i, -i        [Default: 15]             An integer variable\n\t--s, -s        [Default: \"\"]             A string variable\n\t--variable, -v [Default: 15]             A long and short variable";
		EXPECT_EQ(helpText, GETHELPTEXT());
	}
	{
		const char * buffer[] = { "-h" };
		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, 1));
		EXPECT_TRUE(HELPSET());
		std::string helpText = "\t--i, -i        [Default: 15]             An integer variable\n\t--s, -s        [Default: \"\"]             A string variable\n\t--variable, -v [Default: 15]             A long and short variable";
		EXPECT_EQ(helpText, GETHELPTEXT());
	}
}

TEST(ArgumentParser, parseCommandLine) {
	REGISTER_VARIABLE(std::string, s, s, "", "A string variable");
	REGISTER_VARIABLE(int, i, i, 15, "An integer variable");
	const char * buffer[] = { "-/program", "-s", "foobar", "-i", "42" };
	const char ** argv = buffer;
	int argc = 5;
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_COMMANDLINE());
	EXPECT_FALSE(HELPSET());
	EXPECT_EQ("foobar", s);
	EXPECT_EQ(42, i);
}

TEST(ArgumentParser, parseBoolToggleOff) {
	{
		REGISTER_VARIABLE(bool, b, b, false, "A test boolean");
		REGISTER_VARIABLE(bool, d, d, true, "A test boolean");

		const char * buffer[] = { "-b", "-d" };
		int length = sizeof(buffer) / sizeof(char *);

		EXPECT_FALSE(b);
		EXPECT_TRUE(d);

		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, length));

		EXPECT_TRUE(b);
		EXPECT_TRUE(d);
	}
	{
		REGISTER_VARIABLE(bool, b, b, false, "A test boolean");
		REGISTER_VARIABLE(bool, d, d, true, "A test boolean");

		const char * buffer[] = { "-b", "false" };
		int length = sizeof(buffer) / sizeof(char *);

		EXPECT_FALSE(b);
		EXPECT_TRUE(d);

		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, length));

		EXPECT_FALSE(b);
		EXPECT_TRUE(d);
	}
	{
		REGISTER_VARIABLE(bool, b, b, false, "A test boolean");
		REGISTER_VARIABLE(bool, d, d, true, "A test boolean");

		const char * buffer[] = { "-b", "true", "-d", "false" };
		int length = sizeof(buffer) / sizeof(char *);

		EXPECT_FALSE(b);
		EXPECT_TRUE(d);

		EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, length));

		EXPECT_TRUE(b);
		EXPECT_FALSE(d);
	}
}

TEST(ArgumentParser, definingListBeingDestroyedBeforeUsage) {
	REGISTER_VARIABLE(int32_t, i, i, -1, "A test integer");
	{
		REGISTER_VARIABLE_ARGUMENTS(args);
	}
	const char * buffer1[] = { "a1", "a2", "a3", "a4", "a5", "a6" };
	int length1 = sizeof(buffer1) / sizeof(char *);
	EXPECT_EQ(clockUtils::ClockError::INVALID_USAGE, PARSE_ARGUMENTS(buffer1, length1));
}

TEST(ArgumentParser, longAndShortName) {
	REGISTER_VARIABLE(std::string, string, s, "", "Sample");
	REGISTER_VARIABLE(int, integer, i, 15, "Sample");
	const char * buffer[] = { "-s", "foo", "--integer", "42" };
	EXPECT_EQ(clockUtils::ClockError::SUCCESS, PARSE_ARGUMENTS(buffer, 4));
	EXPECT_EQ("foo", string);
	EXPECT_EQ(42, integer);
}
