#include "clockUtils/errors.h"
#include "clockUtils/iniParser/iniParser.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::iniParser;

TEST(IniParser, loadInvalid) {
	IniParser i1;
	EXPECT_EQ(ClockError::FILENOTFOUND, i1.load("NotExistentFile.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectSection.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/NoSection.ini"));
	EXPECT_EQ(ClockError::WRONG_SYNTAX, i1.load("resources/DefectField.ini"));
}
