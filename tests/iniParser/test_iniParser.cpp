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

#include <fstream>

#include "clockUtils/errors.h"
#include "clockUtils/iniParser/iniParser.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::iniParser;

TEST(IniParser, loadInvalidFile) {
	IniParser i1;
	EXPECT_EQ(ClockError::FILENOTFOUND, i1.load("NotExistentFile.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectSection.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectField.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectA.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectB.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectC.ini"));
}

TEST(IniParser, loadTwoFiles) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example1.ini"));
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example2.ini"));
	double s1e2;
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue<double>("SECTION1", "entry2", s1e2));
}

TEST(IniParser, getValue) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example1.ini"));

	int s1e1;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<int>("SECTION1", "entry1", s1e1));
	EXPECT_EQ(5, s1e1);
	double s1e2;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<double>("SECTION1", "entry2", s1e2));
	EXPECT_DOUBLE_EQ(12, s1e2);
	std::string s1e3;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<std::string>("SECTION1", "entry3", s1e3));
	EXPECT_EQ("English", s1e3);
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue<std::string>("SECTION1", "entry4", s1e3));
	EXPECT_EQ(ClockError::WRONG_TYPE, i1.getValue<int>("SECTION1", "entry3", s1e1));
	EXPECT_EQ(ClockError::WRONG_TYPE, i1.getValue<double>("SECTION1", "entry3", s1e2));

	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<int>("SECTION2", "entry3", s1e1));

	EXPECT_EQ(-100, s1e1);
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<double>("SECTION2", "entry2", s1e2));
	EXPECT_DOUBLE_EQ(3.141592, s1e2);
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<std::string>("SECTION2", "entry1", s1e3));
	EXPECT_EQ("German", s1e3);
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue<std::string>("SECTION2", "entry4", s1e3));
	EXPECT_EQ(ClockError::WRONG_TYPE, i1.getValue<int>("SECTION2", "entry1", s1e1));
	EXPECT_EQ(ClockError::WRONG_TYPE, i1.getValue<double>("SECTION2", "entry1", s1e2));
}

struct Vec3 {
	double x, y, z;
	Vec3 & operator=(const Vec3 &) = delete;
	friend std::istream & operator>>(std::istream & in, Vec3 & vec) {
		in >> vec.x >> vec.y >> vec.z;
		return in;
	}
};

TEST(IniParser, getUserDefined) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/exampleVector.ini"));
	Vec3 v;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<Vec3>("SECTION1", "pointA", v));
	EXPECT_DOUBLE_EQ(1.5, v.x);
	EXPECT_DOUBLE_EQ(2.3, v.y);
	EXPECT_DOUBLE_EQ(0.7, v.z);
	EXPECT_EQ(ClockError::WRONG_TYPE, i1.getValue<Vec3>("SECTION1", "pointB", v));
	EXPECT_EQ(ClockError::WRONG_TYPE, i1.getValue<Vec3>("SECTION1", "pointC", v));
}

TEST(IniParser, loadSave) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example1.ini"));
	EXPECT_EQ(ClockError::SUCCESS, i1.save("resources/example1saved.ini"));

	std::fstream fs1;
	fs1.open("resources/example1.ini", std::fstream::in);
	std::fstream fs2;
	fs2.open("resources/example1saved.ini", std::fstream::in);

	while (fs1.good() && fs2.good()) {
		std::string line1 = "";
		getline(fs1, line1);
		std::string line2 = "";
		getline(fs2, line2);
		EXPECT_EQ(line1, line2);
	}

	EXPECT_EQ(fs1.good(), fs2.good());
}

TEST(IniParser, setNewValue) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example1.ini"));

	int s1e1;
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue("SECTION1", "entry4", s1e1));
	s1e1 = 11;
	i1.setValue("SECTION1", "entry4", s1e1);
	s1e1 = 0;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue("SECTION1", "entry4", s1e1));
	EXPECT_EQ(11, s1e1);

	EXPECT_EQ(ClockError::SUCCESS, i1.save("resources/example1saved2.ini"));

	std::fstream fs1;
	fs1.open("resources/example1result.ini", std::fstream::in);
	std::fstream fs2;
	fs2.open("resources/example1saved2.ini", std::fstream::in);

	while (fs1.good() && fs2.good()) {
		std::string line1 = "";
		getline(fs1, line1);
		std::string line2 = "";
		getline(fs2, line2);
		EXPECT_EQ(line1, line2);
	}

	EXPECT_EQ(fs1.good(), fs2.good());
}

TEST(IniParser, setNewSection) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example1.ini"));

	int s3e1;
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue<int>("SECTION3", "entry1", s3e1));
	s3e1 = 123;
	i1.setValue<int>("SECTION3", "entry1", s3e1);
	s3e1 = 0;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<int>("SECTION3", "entry1", s3e1));
	EXPECT_EQ(123, s3e1);
	std::string s3e2;
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue<std::string>("SECTION3", "entry2", s3e2));
	s3e2 = "blafoo";
	i1.setValue<std::string>("SECTION3", "entry2", s3e2);
	s3e2 = "";
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<std::string>("SECTION3", "entry2", s3e2));
	EXPECT_EQ("blafoo", s3e2);

	EXPECT_EQ(ClockError::SUCCESS, i1.save("resources/example1saved3.ini"));

	std::fstream fs1;
	fs1.open("resources/example1result2.ini", std::fstream::in);
	std::fstream fs2;
	fs2.open("resources/example1saved3.ini", std::fstream::in);

	while (fs1.good() && fs2.good()) {
		std::string line1 = "";
		getline(fs1, line1);
		std::string line2 = "";
		getline(fs2, line2);
		EXPECT_EQ(line1, line2);
	}

	EXPECT_EQ(fs1.good(), fs2.good());
}

TEST(IniParser, getValueWithSpaces) {
	IniParser i1;
	EXPECT_EQ(ClockError::SUCCESS, i1.load("resources/example2.ini"));

	std::string s1e1;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue("SECTION1", "entry1", s1e1));
	EXPECT_EQ("5 0 0", s1e1);

	std::string str;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<std::string>("SECTION1", "string", str));
	EXPECT_EQ("hello world;", str);
}

TEST(IniParser, enumValues) {
	enum TestEnumA {
		ValA1,
		ValA2,
		ValA3
	};
	enum class TestEnumB {
		ValB1,
		ValB2,
		ValB3
	};
	IniParser i;
	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/exampleEnum.ini"));

	TestEnumA v1;
	EXPECT_EQ(ClockError::SUCCESS, i.getValue("SECTION1", "enumA1", v1));
	EXPECT_EQ(TestEnumA::ValA1, v1);
	EXPECT_EQ(ClockError::SUCCESS, i.getValue("SECTION1", "enumA2", v1));
	EXPECT_EQ(TestEnumA::ValA2, v1);

	TestEnumB v2;
	EXPECT_EQ(ClockError::SUCCESS, i.getValue("SECTION1", "enumB1", v2));
	EXPECT_EQ(TestEnumB::ValB1, v2);
	EXPECT_EQ(ClockError::SUCCESS, i.getValue("SECTION1", "enumB2", v2));
	EXPECT_EQ(TestEnumB::ValB2, v2);

	TestEnumB v3 = TestEnumB::ValB3;
	i.setValue("SECTION1", "enumB3", v3);
	EXPECT_EQ(ClockError::SUCCESS, i.getValue("SECTION1", "enumB3", v2));
	EXPECT_EQ(TestEnumB::ValB3, v2);
}

TEST(IniParser, GetAllSections) {
	IniParser i;
	std::vector<std::string> sections = i.getAllSections();
	ASSERT_TRUE(sections.empty());

	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/example1.ini"));
	sections = i.getAllSections();
	ASSERT_EQ(3, sections.size());
	EXPECT_EQ("SECTION1", sections[0]);
	EXPECT_EQ("SECTION2", sections[1]);
	EXPECT_EQ("global", sections[2]);

	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/example1result2.ini"));
	sections = i.getAllSections();
	ASSERT_EQ(4, sections.size());
	EXPECT_EQ("SECTION1", sections[0]);
	EXPECT_EQ("SECTION2", sections[1]);
	EXPECT_EQ("SECTION3", sections[2]);
	EXPECT_EQ("global", sections[3]);

	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/example2.ini"));
	sections = i.getAllSections();
	ASSERT_EQ(2, sections.size());
	EXPECT_EQ("SECTION1", sections[0]);
	EXPECT_EQ("global", sections[1]);

	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/NoSection.ini"));
	sections = i.getAllSections();
	ASSERT_EQ(1, sections.size());
	EXPECT_EQ("global", sections[0]);
}

TEST(IniParser, GetAllEntries) {
	IniParser i;
	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/example3.ini"));
	std::vector<std::string> sections = i.getAllSections();
	ASSERT_EQ(3, sections.size());
	EXPECT_EQ("SECTION1", sections[0]);
	EXPECT_EQ("SECTION2", sections[1]);
	EXPECT_EQ("global", sections[2]);

	std::vector<std::string> entries = i.getAllEntries("global");
	ASSERT_EQ(1, entries.size());
	EXPECT_EQ("entryWithoutSection", entries[0]);

	entries = i.getAllEntries("SECTION1");
	ASSERT_EQ(2, entries.size());
	EXPECT_EQ("entry1", entries[0]);
	EXPECT_EQ("entry3", entries[1]);

	entries = i.getAllEntries("SECTION2");
	ASSERT_EQ(2, entries.size());
	EXPECT_EQ("entry4", entries[0]);
	EXPECT_EQ("entry2", entries[1]);
}

TEST(IniParser, RemoveEntry) {
	IniParser i;
	EXPECT_EQ(ClockError::SUCCESS, i.load("resources/example3.ini"));

	i.removeEntry("SECTION1", "entry1");

	std::string entry;
	EXPECT_NE(ClockError::SUCCESS, i.getValue("SECTION1", "entry1", entry));

	i.removeEntry("SECTION1", "entry3");

	EXPECT_NE(ClockError::SUCCESS, i.getValue("SECTION1", "entry3", entry));

	EXPECT_EQ(2, i.getAllSections().size());
}
