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

#include "clockUtils/container/DoubleBufferQueue.h"

#include <thread>

#include "gtest/gtest.h"

using clockUtils::ClockError;
using clockUtils::container::DoubleBufferQueue;

TEST(DoubleBufferQueue, Simple) {
	DoubleBufferQueue<int, false, false> q;
	EXPECT_TRUE(q.empty());
	EXPECT_EQ(0, q.size());
}

TEST(DoubleBufferQueue, Pushing) {
	DoubleBufferQueue<int, false, false> q;
	for (int i = 0; i < 10; ++i) {
		q.push(i);
		EXPECT_FALSE(q.empty());
		EXPECT_EQ(i + 1, q.size());
	}
}

TEST(DoubleBufferQueue, PushPop) {
	DoubleBufferQueue<int, false, false> q;
	int value;
	EXPECT_EQ(ClockError::NO_ELEMENT, q.front(value));
	EXPECT_EQ(ClockError::NO_ELEMENT, q.pop());
	for (int i = 0; i < 10; ++i) {
		q.push(i);
		EXPECT_FALSE(q.empty());
		EXPECT_EQ(i + 1, q.size());
		EXPECT_EQ(ClockError::SUCCESS, q.front(value));
		EXPECT_EQ(0, value);
	}
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q.front(value));
		EXPECT_EQ(i, value);
		EXPECT_EQ(ClockError::SUCCESS, q.pop());
		EXPECT_EQ(9 - i, q.size());
	}
	EXPECT_TRUE(q.empty());
}

TEST(DoubleBufferQueue, Poll) {
	DoubleBufferQueue<int, false, false> q;
	for (int i = 0; i < 10; ++i) {
		q.push(i);
		EXPECT_FALSE(q.empty());
		EXPECT_EQ(i + 1, q.size());
		int value;
		EXPECT_EQ(ClockError::SUCCESS, q.front(value));
		EXPECT_EQ(0, value);
	}
	for (int i = 0; i < 10; ++i) {
		int value;
		ClockError err = q.poll(value);
		EXPECT_EQ(ClockError::SUCCESS, err);
		EXPECT_EQ(i, value);
		EXPECT_EQ(9 - i, q.size());
	}
	EXPECT_TRUE(q.empty());
}

TEST(DoubleBufferQueue, PushPoll2) {
	DoubleBufferQueue<int, false, false> q;
	for (int i = 0; i < 5; ++i) {
		q.push(7);
		EXPECT_EQ(1, q.size());
		q.push(56);
		EXPECT_EQ(2, q.size());
		int value;
		EXPECT_EQ(ClockError::SUCCESS, q.poll(value));
		EXPECT_EQ(7, value);
		EXPECT_EQ(1, q.size());
		q.push(23);
		EXPECT_EQ(2, q.size());
		EXPECT_EQ(ClockError::SUCCESS, q.poll(value));
		EXPECT_EQ(56, value);
		EXPECT_EQ(ClockError::SUCCESS, q.poll(value));
		EXPECT_EQ(23, value);
		EXPECT_TRUE(q.empty());
	}
	int value;
	EXPECT_EQ(ClockError::NO_ELEMENT, q.poll(value));
}

TEST(DoubleBufferQueue, Clear) {
	DoubleBufferQueue<int, false, false> q;
	q.clear();
	for (int i = 0; i < 5; ++i) {
		q.push(13);
		q.push(37);
		q.push(23);
		q.push(7);
		q.push(42);
		EXPECT_EQ(5, q.size());
		EXPECT_FALSE(q.empty());
		q.clear();
		EXPECT_EQ(0, q.size());
		EXPECT_TRUE(q.empty());
	}
}

void pusher(DoubleBufferQueue<int, true, true> * q, int amount, int value) {
	for (int i = 0; i < amount; ++i) {
		q->push(value);
	}
}

void popper(DoubleBufferQueue<int, true, true> * qFrom, DoubleBufferQueue<int, true, false> * qTo, int amount) {
	for (int i = 0; i < amount; ++i) {
		if (!qFrom->empty()) {
			int a;
			ClockError err = qFrom->poll(a);
			if (err == ClockError::SUCCESS) {
				qTo->push(a);
			} else {
				i--;
			}
		} else {
			i--;
		}
	}
}

TEST(DoubleBufferQueue, StressTest) {
	DoubleBufferQueue<int, true, true> q1;
	DoubleBufferQueue<int, true, false> q2;
	std::vector<std::thread *> v;
	for (int i = 0; i < 40; ++i) {
		v.push_back(new std::thread(std::bind(pusher, &q1, 1000, i)));
	}
	for (int i = 0; i < 80; ++i) {
		v.push_back(new std::thread(std::bind(popper, &q1, &q2, 500)));
	}
	std::vector<int> counts(40);
	for (unsigned int i = 0; i < 120; ++i) {
		v[i]->join();
		delete v[i];
	}
	for (int i = 0; i < 40 * 1000; ++i) {
		int a = 0;
		EXPECT_EQ(ClockError::SUCCESS, q2.poll(a));
		counts[size_t(a)]++;
	}
	for (unsigned int i = 0; i < 40; ++i) {
		EXPECT_EQ(1000, counts[i]);
	}
	EXPECT_TRUE(q1.empty());
	EXPECT_TRUE(q2.empty());
}
