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

#define _GLIBCXX_USE_NANOSLEEP // needed for sleep_for, see http://stackoverflow.com/questions/4438084/stdthis-threadsleep-for-and-gcc

#include <chrono>
#include <thread>

#include "clockUtils/errors.h"
#include "clockUtils/sockets/UdpSocket.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::sockets;

TEST(UdpSocket, bind) { // tests binding a socket to a port
	UdpSocket server;
	UdpSocket server2;

	EXPECT_EQ(ClockError::SUCCESS, server.bind(12345));
	EXPECT_EQ(ClockError::INVALID_USAGE, server.bind(12345));
	EXPECT_EQ(ClockError::ADDRESS_INUSE, server2.bind(12345));

	std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

TEST(UdpSocket, sendRead) { // tests communication between two sockets
	UdpSocket sock1;
	UdpSocket sock2;

	std::string message1 = "Hello World!";
	std::string message2 = "Some other message!";

	EXPECT_EQ(ClockError::SUCCESS, sock1.bind(12345));
	EXPECT_EQ(ClockError::SUCCESS, sock2.bind(12346));

	EXPECT_EQ(ClockError::SUCCESS, sock1.writePacket("127.0.0.1", 12346, message1.c_str(), message1.length()));

	std::string result;
	std::string ip;
	uint16_t port;

	EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(result, ip, port));
	EXPECT_EQ(message1, result);
	EXPECT_EQ("127.0.0.1", ip);
	EXPECT_EQ(12345, port);

	EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket("127.0.0.1", 12345, message2.c_str(), message2.length()));

	EXPECT_EQ(ClockError::SUCCESS, sock1.receivePacket(result, ip, port));
	EXPECT_EQ(message2, result);
	EXPECT_EQ("127.0.0.1", ip);
	EXPECT_EQ(12346, port);
}

TEST(UdpSocket, useUnready) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;
	std::string ip;
	uint16_t port;

	UdpSocket sock1;
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket("127.0.0.1", 12345, buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket("127.0.0.1", 12345, reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.write("127.0.0.1", 12345, reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer, ip, port));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(str, ip, port));

	std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

TEST(UdpSocket, closeFails) {
	UdpSocket sock1;
	sock1.close();
	sock1.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

TEST(UdpSocket, writePacketMultiple) {
	UdpSocket sock1, sock2;
	std::vector<uint8_t> v1 = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1 };
	std::vector<uint8_t> v2 = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11 };

	sock1.bind(12345);
	sock2.bind(12346);

	sock1.writePacket("127.0.0.1", 12346, v1);
	sock1.writePacket("127.0.0.1", 12346, v2);

	std::string ip;
	uint16_t port;

	std::vector<uint8_t> v3, v4;
	sock2.receivePacket(v3, ip, port);
	EXPECT_EQ(v1, v3);
	sock2.receivePacket(v4, ip, port);
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();
}

TEST(UdpSocket, writeMass) {
	UdpSocket sock1, sock2;
	std::vector<uint8_t> v1(100000, 'a');
	std::vector<uint8_t> v2(100000, 'b');

	ASSERT_EQ(ClockError::SUCCESS, sock1.bind(12345));
	ASSERT_EQ(ClockError::SUCCESS, sock2.bind(12346));
	ASSERT_EQ(ClockError::SUCCESS, sock1.writePacket("127.0.0.1", 12346, v1));
	ASSERT_EQ(ClockError::SUCCESS, sock1.writePacket("127.0.0.1", 12346, v2));

	std::string ip;
	uint16_t port;

	std::vector<uint8_t> v3, v4;
	EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(v3, ip, port));
	EXPECT_EQ(v1, v3);
	EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(v4, ip, port));
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();
}

TEST(UdpSocket, receiveCallback) {
	UdpSocket sock1, sock2;

	std::vector<std::string> messages = { "Hi" };
	int received = 0;

	sock1.bind(12345);
	sock1.receiveCallback([&received](std::vector<uint8_t> packet, std::string ip, uint16_t port, ClockError err) {
		received++;
	});
	sock2.bind(12346);

	for (std::string s : messages) {
		EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket("127.0.0.1", 12345, s.c_str(), s.length()));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	EXPECT_EQ(messages.size(), received);

	sock1.close();
	sock2.close();
}

TEST(UdpSocket, receiveCallbackRemove) {
	UdpSocket sock1, sock2;
	int called = 0;

	sock1.bind(12345);
	sock1.receiveCallback([&called](std::vector<uint8_t> packet, std::string ip, uint16_t port, ClockError err) {
		called++;
		if (err != ClockError::SUCCESS) {
			EXPECT_EQ(2, called);
		} else {
			EXPECT_EQ(1, called);
		}
	});
	sock2.bind(12346);


	std::string s = "Some messsage!";

	EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket("127.0.0.1", 12345, s.c_str(), s.length()));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	sock1.close();
	sock2.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	EXPECT_EQ(2, called);
}

TEST(UdpSocket, writePacketAsyncMultiple) {
	UdpSocket sock1, sock2;
	std::vector<uint8_t> v1 = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1 };
	std::vector<uint8_t> v2 = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11 };

	sock1.bind(12345);
	sock2.bind(12346);

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	sock1.writePacketAsync("127.0.0.1", 12346, v1);
	sock1.writePacketAsync("127.0.0.1", 12346, v2);

	std::string ip;
	uint16_t port;

	std::vector<uint8_t> v3, v4;
	sock2.receivePacket(v3, ip, port);
	EXPECT_EQ(v1, v3);
	sock2.receivePacket(v4, ip, port);
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();
}

TEST(UdpSocket, writePacketAsyncMass) {
	UdpSocket sock1, sock2;
	std::vector<uint8_t> v1(100000, 'a');
	std::vector<uint8_t> v2(100000, 'b');

	ASSERT_EQ(ClockError::SUCCESS, sock1.bind(12345));
	ASSERT_EQ(ClockError::SUCCESS, sock2.bind(12346));
	ASSERT_EQ(ClockError::SUCCESS, sock1.writePacketAsync("127.0.0.1", 12346, v1));
	ASSERT_EQ(ClockError::SUCCESS, sock1.writePacketAsync("127.0.0.1", 12346, v2));

	std::string ip;
	uint16_t port;

	std::vector<uint8_t> v3, v4;
	EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(v3, ip, port));
	EXPECT_EQ(v1, v3);
	EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(v4, ip, port));
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();
}
