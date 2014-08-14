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
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/NoSection.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectField.ini"));
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
	EXPECT_EQ(ClockError::VALUE_NOTFOUND, i1.getValue<int>("SECTION1", "entry4", s1e1));
	s1e1 = 11;
	i1.setValue<int>("SECTION1", "entry4", s1e1);
	s1e1 = 0;
	EXPECT_EQ(ClockError::SUCCESS, i1.getValue<int>("SECTION1", "entry4", s1e1));
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
}
