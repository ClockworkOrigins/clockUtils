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

#include "clockUtils/log/Log.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::log;

class LogTest : public ::testing::Test {
	// registers test as application and ObjectController
	virtual void SetUp() override {
		Logger::reset();
		Logger::setLogLevel(LogLevel::LOG_DEBUG);
	}

	virtual void TearDown() override {
		Logger::reset();
	}
};

TEST_F(LogTest, addSink) {
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&std::cout));
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&std::cerr));
	ASSERT_EQ(ClockError::SINK_ALREADY_INSERTED, Logger::addSink(&std::cerr));

	std::ofstream out;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&out));

	std::stringstream ss;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss));
	ASSERT_EQ(ClockError::SINK_ALREADY_INSERTED, Logger::addSink(&ss));

	std::vector<int> a;
	ASSERT_EQ(ClockError::NO_VALID_SINK, Logger::addSink(&a));
}

TEST_F(LogTest, log) {
	std::stringstream ss;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss));

	Logger::log(LogLevel::LOG_DEBUG, "Foobar");

	ASSERT_EQ("Foobar\n", ss.str());

	Logger::log(LogLevel::LOG_INFO, "Blafoo");

	ASSERT_EQ("Foobar\nBlafoo\n", ss.str());
}

TEST_F(LogTest, logMultipleSinks) {
	std::stringstream ss;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss));
	std::stringstream ss2;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss2));

	Logger::log(LogLevel::LOG_DEBUG, "Foobar");

	ASSERT_EQ("Foobar\n", ss.str());
	ASSERT_EQ("Foobar\n", ss2.str());

	Logger::log(LogLevel::LOG_INFO, "Blafoo");

	ASSERT_EQ("Foobar\nBlafoo\n", ss.str());
	ASSERT_EQ("Foobar\nBlafoo\n", ss2.str());
}

TEST_F(LogTest, reset) {
	std::stringstream ss;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss));
	std::stringstream ss2;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss2));

	Logger::log(LogLevel::LOG_DEBUG, "Foobar");

	ASSERT_EQ("Foobar\n", ss.str());
	ASSERT_EQ("Foobar\n", ss2.str());

	Logger::reset();

	Logger::log(LogLevel::LOG_INFO, "Blafoo");

	ASSERT_EQ("Foobar\n", ss.str());
	ASSERT_EQ("Foobar\n", ss2.str());
}

TEST_F(LogTest, setLogLevel) {
	std::stringstream ss;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss));
	std::stringstream ss2;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ss2));

	Logger::log(LogLevel::LOG_DEBUG, "Foobar");

	ASSERT_EQ("Foobar\n", ss.str());
	ASSERT_EQ("Foobar\n", ss2.str());

	Logger::setLogLevel(LogLevel::LOG_ERROR);

	Logger::log(LogLevel::LOG_INFO, "clockUtils is nice");

	ASSERT_EQ("Foobar\n", ss.str());
	ASSERT_EQ("Foobar\n", ss2.str());

	Logger::log(LogLevel::LOG_ERROR, "Blafoo");

	ASSERT_EQ("Foobar\nBlafoo\n", ss.str());
	ASSERT_EQ("Foobar\nBlafoo\n", ss2.str());
}

TEST_F(LogTest, logStream) {
	std::stringstream ssSink;
	ASSERT_EQ(ClockError::SUCCESS, Logger::addSink(&ssSink));

	LOGINFO("Foobar " << 2);

	ASSERT_EQ("Foobar 2\n", ssSink.str());
}
