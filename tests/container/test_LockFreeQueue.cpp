/*
 * clockUtils
 * Copyright (2016) Michael Baer, Daniel Bonrath, All rights reserved.
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

#include "clockUtils/container/LockFreeQueue.h"

#include <functional>
#include <thread>

#include "gtest/gtest.h"

using clockUtils::ClockError;
using clockUtils::container::LockFreeQueue;

TEST(LockFreeQueue, Simple) {
	LockFreeQueue<int, 1> q;
	EXPECT_TRUE(q.empty());
	EXPECT_EQ(0, q.size());
}

TEST(LockFreeQueue, Pushing) {
	LockFreeQueue<int, 10> q;
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q.push(i));
		EXPECT_FALSE(q.empty());
		EXPECT_EQ(i + 1, q.size());
	}
}

TEST(LockFreeQueue, PushPop) {
	LockFreeQueue<int, 10> q;
	int value;
	EXPECT_EQ(ClockError::NO_ELEMENT, q.front(value));
	EXPECT_EQ(ClockError::NO_ELEMENT, q.pop());
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q.push(i));
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

TEST(LockFreeQueue, Poll) {
	LockFreeQueue<int, 10> q;
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q.push(i));
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

TEST(LockFreeQueue, PushPoll2) {
	LockFreeQueue<int, 3> q;
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q.push(7));
		EXPECT_EQ(1, q.size());
		EXPECT_EQ(ClockError::SUCCESS, q.push(56));
		EXPECT_EQ(2, q.size());
		int value;
		EXPECT_EQ(ClockError::SUCCESS, q.poll(value));
		EXPECT_EQ(7, value);
		EXPECT_EQ(1, q.size());
		EXPECT_EQ(ClockError::SUCCESS, q.push(23));
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

TEST(LockFreeQueue, Clear) {
	LockFreeQueue<int, 5> q;
	q.clear();
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q.push(13));
		EXPECT_EQ(ClockError::SUCCESS, q.push(37));
		EXPECT_EQ(ClockError::SUCCESS, q.push(23));
		EXPECT_EQ(ClockError::SUCCESS, q.push(7));
		EXPECT_EQ(ClockError::SUCCESS, q.push(42));
		EXPECT_EQ(5, q.size());
		EXPECT_FALSE(q.empty());
		q.clear();
		EXPECT_EQ(0, q.size());
		EXPECT_TRUE(q.empty());
	}
}

const int PUSH_THREADS = 40;
const int AMOUNT = 1000;

template<typename T>
void pusher(T * q, int amount, int value) {
	for (int i = 0; i < amount; ++i) {
		EXPECT_EQ(ClockError::SUCCESS, q->push(value));
	}
}

template<typename T>
void popper(T * qFrom, T * qTo, int amount) {
	for (int i = 0; i < amount; ++i) {
		if (!qFrom->empty()) {
			int a;
			ClockError err = qFrom->poll(a);
			if (err == ClockError::SUCCESS) {
				EXPECT_EQ(ClockError::SUCCESS, qTo->push(a));
			} else {
				i--;
			}
		} else {
			i--;
		}
	}
}

TEST(LockFreeQueue, StressTest) {
	LockFreeQueue<int, PUSH_THREADS * AMOUNT> q1;
	LockFreeQueue<int, PUSH_THREADS * AMOUNT> q2;
	std::vector<std::thread *> v;
	for (int i = 0; i < PUSH_THREADS; ++i) {
		v.push_back(new std::thread(std::bind(pusher<LockFreeQueue<int, PUSH_THREADS * AMOUNT>>, &q1, AMOUNT, i)));
	}
	for (int i = 0; i < PUSH_THREADS * 2; ++i) {
		v.push_back(new std::thread(std::bind(popper<LockFreeQueue<int, PUSH_THREADS * AMOUNT>>, &q1, &q2, AMOUNT / 2)));
	}
	std::vector<int> counts(PUSH_THREADS);
	for (size_t i = 0; i < v.size(); ++i) {
		v[i]->join();
		delete v[i];
	}
	for (int i = 0; i < PUSH_THREADS * AMOUNT; ++i) {
		int a = 0;
		EXPECT_EQ(ClockError::SUCCESS, q2.poll(a));
		counts[size_t(a)]++;
	}
	for (int i = 0; i < PUSH_THREADS; ++i) {
		EXPECT_EQ(AMOUNT, counts[i]);
	}
	EXPECT_TRUE(q1.empty());
	EXPECT_TRUE(q2.empty());
}

const int PUSH_THREADS2 = 4;
const int AMOUNT2 = 100000;

TEST(LockFreeQueue, StressTest2) {
	LockFreeQueue<int, PUSH_THREADS2 * AMOUNT2> * q1 = new LockFreeQueue<int, PUSH_THREADS2 * AMOUNT2>();
	LockFreeQueue<int, PUSH_THREADS2 * AMOUNT2> * q2 = new LockFreeQueue<int, PUSH_THREADS2 * AMOUNT2>();
	std::vector<std::thread *> v;
	for (int i = 0; i < PUSH_THREADS2; ++i) {
		v.push_back(new std::thread(std::bind(pusher<LockFreeQueue<int, PUSH_THREADS2 * AMOUNT2>>, q1, AMOUNT2, i)));
	}
	for (int i = 0; i < PUSH_THREADS2 * 2; ++i) {
		v.push_back(new std::thread(std::bind(popper<LockFreeQueue<int, PUSH_THREADS2 * AMOUNT2>>, q1, q2, AMOUNT2 / 2)));
	}
	std::vector<int> counts(PUSH_THREADS2);
	for (size_t i = 0; i < v.size(); ++i) {
		v[i]->join();
		delete v[i];
	}
	for (int i = 0; i < PUSH_THREADS2 * AMOUNT2; ++i) {
		int a = 0;
		EXPECT_EQ(ClockError::SUCCESS, q2->poll(a));
		counts[size_t(a)]++;
	}
	for (int i = 0; i < PUSH_THREADS2; ++i) {
		EXPECT_EQ(AMOUNT2, counts[i]);
	}
	EXPECT_TRUE(q1->empty());
	EXPECT_TRUE(q2->empty());

	delete q1;
	delete q2;
}

TEST(LockFreeQueue, StressTestSingleProducerSingleConsumer) {
	LockFreeQueue<int, AMOUNT2> * q1 = new LockFreeQueue<int, AMOUNT2>();
	LockFreeQueue<int, AMOUNT2> * q2 = new LockFreeQueue<int, AMOUNT2>();
	std::vector<std::thread *> v;
	v.push_back(new std::thread(std::bind(pusher<LockFreeQueue<int, AMOUNT2>>, q1, AMOUNT2, 42)));
	v.push_back(new std::thread(std::bind(popper<LockFreeQueue<int, AMOUNT2>>, q1, q2, AMOUNT2)));
	int counts = 0;
	for (size_t i = 0; i < v.size(); ++i) {
		v[i]->join();
		delete v[i];
	}
	for (int i = 0; i < AMOUNT2; ++i) {
		int a = 0;
		EXPECT_EQ(ClockError::SUCCESS, q2->poll(a));
		EXPECT_EQ(42, a);
		counts++;
	}
	EXPECT_EQ(AMOUNT2, counts);
	EXPECT_TRUE(q1->empty());
	EXPECT_TRUE(q2->empty());

	delete q1;
	delete q2;
}

TEST(LockFreeQueue, MoreThanSpace) {
	LockFreeQueue<int, 2> q;
	EXPECT_EQ(ClockError::SUCCESS, q.push(0));
	EXPECT_EQ(ClockError::SUCCESS, q.push(1));
	EXPECT_EQ(ClockError::NO_SPACE_AVAILABLE, q.push(2));
}

TEST(LockFreeQueue, MoreThanSpaceWithPop) {
	LockFreeQueue<int, 2> q;
	EXPECT_EQ(ClockError::SUCCESS, q.push(0));
	EXPECT_EQ(ClockError::SUCCESS, q.push(1));
	EXPECT_EQ(ClockError::SUCCESS, q.pop());
	EXPECT_EQ(ClockError::SUCCESS, q.push(2));
}
