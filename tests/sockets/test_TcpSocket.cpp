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
#include "clockUtils/sockets/TcpSocket.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::sockets;

int connectCounter = 0;
std::mutex connectionLock;
std::condition_variable conditionVariable;
std::vector<std::string> messages = { "Hello", "World!", "This is a super nice message", "111elf!!!" };
std::vector<TcpSocket *> _socketList;

void connectionAccepted(TcpSocket * sock, ClockError error) {
	if (error == ClockError::SUCCESS) {
		std::unique_lock<std::mutex> ul(connectionLock);
		connectCounter++;
		_socketList.push_back(sock);
		conditionVariable.notify_one();
	}
}

void connectionAcceptedWrite(TcpSocket * ts, ClockError) {
	unsigned int counter = 0;

	while (counter < messages.size()) {
		std::string buffer;

		ClockError e = ts->receivePacket(buffer);

		EXPECT_EQ(ClockError::SUCCESS, e);
		EXPECT_EQ(messages[counter], buffer);

		e = ts->writePacket(buffer.c_str(), buffer.length());

		EXPECT_EQ(ClockError::SUCCESS, e);

		counter++;
	}

	ts->close();
	delete ts;
}

size_t receivedCounter = 0;

void receiveMessage(const std::vector<uint8_t> & message, TcpSocket *, ClockError error) {
	if (receivedCounter == messages.size()) {
		EXPECT_EQ(ClockError::NOT_CONNECTED, error);
	} else {
		ASSERT_EQ(ClockError::SUCCESS, error);
		EXPECT_EQ(messages[receivedCounter], std::string(message.begin(), message.end()));

		receivedCounter++;
	}
}

TEST(TcpSocket, connect) { // tests connect with all possible errors
	TcpSocket ts;
	ClockError e = ts.connect("1", 12345, 100);

	EXPECT_EQ(ClockError::INVALID_IP, e);
	e = ts.connect("127.0.0.1", 0, 100);

	EXPECT_EQ(ClockError::INVALID_PORT, e);
	e = ts.connect("192.168.255.255", 12345, 100);

	EXPECT_EQ(ClockError::TIMEOUT, e);
	ts.connect("127.0.0.1", 12345, 100);

	EXPECT_EQ(ClockError::TIMEOUT, e);
	TcpSocket server;

	e = server.listen(12345, 1, true, [](TcpSocket * sock, ClockError) {
		std::unique_lock<std::mutex> ul(connectionLock);
		_socketList.push_back(sock);
		conditionVariable.notify_one();
	});

	e = ts.connect("127.0.0.1", 12345, 100);

	EXPECT_EQ(ClockError::SUCCESS, e);
	e = ts.connect("127.0.0.1", 12345, 100);

	EXPECT_EQ(ClockError::INVALID_USAGE, e);
	ts.close();

	{
		std::unique_lock<std::mutex> ul(connectionLock);
		e = ts.connect("127.0.0.1", 12345, 100);
		conditionVariable.wait(ul);
	}

	EXPECT_EQ(ClockError::SUCCESS, e);
	server.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, listen) { // tests incoming connections: one thread listening on a port and five or something like that joining
	TcpSocket server1;

	ClockError e = server1.listen(0, 10, true, std::bind(connectionAccepted, std::placeholders::_1, std::placeholders::_2));

	EXPECT_EQ(ClockError::INVALID_PORT, e);

	e = server1.listen(12345, 10, true, std::bind(connectionAccepted, std::placeholders::_1, std::placeholders::_2));

	EXPECT_EQ(ClockError::SUCCESS, e);

	TcpSocket server2;

	e = server2.listen(12345, 10, true, std::bind(connectionAccepted, std::placeholders::_1, std::placeholders::_2));

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::ADDRESS_INUSE, e);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::SUCCESS, e);
#endif

	server2.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	connectCounter = 0;
	EXPECT_EQ(0, connectCounter);

	TcpSocket client1;
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		e = client1.connect("127.0.0.1", 12345, 500);
		EXPECT_EQ(ClockError::SUCCESS, e);
		conditionVariable.wait(ul);
		EXPECT_EQ(1, connectCounter);
	}

	TcpSocket client2;
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		e = client2.connect("127.0.0.1", 12345, 500);
		EXPECT_EQ(ClockError::SUCCESS, e);
		conditionVariable.wait(ul);
		EXPECT_EQ(2, connectCounter);
	}

	TcpSocket client3;
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		e = client3.connect("127.0.0.1", 12345, 500);
		EXPECT_EQ(ClockError::SUCCESS, e);
		conditionVariable.wait(ul);
		EXPECT_EQ(3, connectCounter);
	}

	server1.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	EXPECT_EQ(3, connectCounter);

	client1.close();
	client2.close();
	client3.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	TcpSocket client6;
	e = client6.connect("127.0.0.1", 12345, 100);

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, e);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, e);
#endif
	EXPECT_EQ(3, connectCounter);

	client6.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}

	_socketList.clear();
}

TEST(TcpSocket, sendRead) { // tests communication between two sockets
	TcpSocket server;
	TcpSocket client;

	std::vector<std::string> results;

	ClockError e = server.listen(12345, 10, false, std::bind(connectionAcceptedWrite, std::placeholders::_1, std::placeholders::_2));

	EXPECT_EQ(ClockError::SUCCESS, e);

	e = client.connect("127.0.0.1", 12345, 500);

	EXPECT_EQ(ClockError::SUCCESS, e);

	for (std::string s : messages) {
		e = client.writePacket(s.c_str(), s.length());
		EXPECT_EQ(ClockError::SUCCESS, e);

		std::string buffer;

		e = client.receivePacket(buffer); // the accept functions is an echo function
		EXPECT_EQ(ClockError::SUCCESS, e);

		EXPECT_EQ(s, buffer);
	}

	std::string errorMessage;

	e = client.read(errorMessage);

	EXPECT_EQ(ClockError::NOT_CONNECTED, e);

	server.close();
	client.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

std::string receivedMessage = "";

TEST(TcpSocket, sendFromServer) { // tests communication between two sockets
	receivedMessage = "";

	TcpSocket server;
	TcpSocket client;

	ClockError e = server.listen(12345, 10, false, [](TcpSocket * ts, ClockError) {
			ClockError e2 = ts->writePacket("test", 4);
			EXPECT_EQ(ClockError::SUCCESS, e2);
			ts->close();
			delete ts;
		});
	EXPECT_EQ(ClockError::SUCCESS, e);

	e = client.connect("127.0.0.1", 12345, 500);
	EXPECT_EQ(ClockError::SUCCESS, e);

	std::vector<uint8_t> recMsg;

	client.receiveCallback([recMsg](const std::vector<uint8_t> & message, TcpSocket *, ClockError) {
			for (uint8_t i : message) {
				receivedMessage += char(i);
			}
		});

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	EXPECT_EQ("test", receivedMessage);

	server.close();
	client.close();

	receivedMessage = "";
}

TEST(TcpSocket, getIP) { // tests IP before and after connection
	TcpSocket ts;
	std::string s2 = ts.getPublicIP();
	std::string s3 = ts.getRemoteIP();

	EXPECT_EQ(0, s2.length());
	EXPECT_EQ(0, s3.length());

	TcpSocket server;
	server.listen(12345, 1, false, [](TcpSocket * client, ClockError) {
		std::unique_lock<std::mutex> ul(connectionLock);
		_socketList.push_back(client);
		std::string s4 = client->getRemoteIP();
		std::string s5 = client->getPublicIP();

		EXPECT_NE(0u, s4.length());
		EXPECT_NE(0u, s5.length());
		EXPECT_EQ("127.0.0.1", s4);
		conditionVariable.notify_one();
	});

	{
		std::unique_lock<std::mutex> ul(connectionLock);
		ts.connect("127.0.0.1", 12345, 500);
		s2 = ts.getPublicIP();
		s3 = ts.getRemoteIP();

		EXPECT_NE(0u, s2.length());
		EXPECT_NE(0u, s3.length());
		conditionVariable.wait(ul);
	}

	ts.close();
	server.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, getPort) { // tests port before and after connection
	TcpSocket ts;
	uint16_t s = ts.getLocalPort();
	uint16_t s2 = ts.getRemotePort();

	EXPECT_EQ(0, s);
	EXPECT_EQ(0, s2);

	TcpSocket server;
	server.listen(12345, 1, false, [](TcpSocket * client, ClockError) {
		std::unique_lock<std::mutex> ul(connectionLock);
		uint16_t s3 = client->getRemotePort();
		uint16_t s4 = client->getLocalPort();

		EXPECT_NE(0, s3);
		EXPECT_EQ(12345, s4);

		client->close();
		delete client;
		conditionVariable.notify_one();
	});

	{
		std::unique_lock<std::mutex> ul(connectionLock);
		ts.connect("127.0.0.1", 12345, 500);
		s = ts.getLocalPort();
		s2 = ts.getRemotePort();

		EXPECT_NE(0, s);
		EXPECT_EQ(12345, s2);
		conditionVariable.wait(ul);
	}

	ts.close();
	server.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, useUnready) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;

	TcpSocket sock1;
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(str));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(str));

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, invalidParameters) {
	TcpSocket sock1;
	TcpSocket::acceptCallback acb;
	EXPECT_EQ(ClockError::INVALID_PORT, sock1.listen(0, 1, true, acb));

	EXPECT_EQ(ClockError::INVALID_PORT, sock1.connect("127.0.0.1", 0, 100));

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, sock1.connect("127.0.0.1", 1, 100));
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, sock1.connect("127.0.0.1", 1, 100));
#endif
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.", 1, 100));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0", 1, 100));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("-12.0.0.1", 1, 100));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("12.-3.0.1", 1, 100));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.1.9", 1, 100));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("256.0.0.1", 1, 100));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.1234.0.1", 1, 100));

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, invalidUse) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;

	TcpSocket sock1;
	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(10252, 1, true, [](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
	}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.listen(1025, 1, true, [](TcpSocket *, ClockError) {}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.connect("127.0.0.1", 1026, 500));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(str));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(str));

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, getterUnused) {
	TcpSocket sock1;
	EXPECT_EQ("", sock1.getRemoteIP());
	EXPECT_EQ(0, sock1.getLocalPort());
	EXPECT_EQ(0, sock1.getRemotePort());

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, closeFails) {
	TcpSocket sock1;
	sock1.close();
	sock1.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, connectOnly) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;
	TcpSocket sock1, sock2;

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		std::unique_lock<std::mutex> ul(connectionLock);
		_socketList.push_back(sock);
		conditionVariable.notify_one();
	});
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		sock2.connect("127.0.0.1", 12345, 500);
		conditionVariable.wait(ul);
	}

	EXPECT_EQ(12345, sock2.getRemotePort());
	EXPECT_EQ(_socketList[0]->getLocalPort(), sock2.getRemotePort());
	EXPECT_EQ(sock2.getLocalPort(), _socketList[0]->getRemotePort());

	sock1.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(10252, 1, true, [](TcpSocket *, ClockError) {}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.listen(1025, 1, true, [](TcpSocket *, ClockError) {}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.connect("127.0.0.1", 1026, 200));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(str));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(str));

	sock2.writePacket(buffer);
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.read(buffer));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.read(str));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.receivePacket(str));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.listen(1026, 1, true, [](TcpSocket *, ClockError) {}));

	sock2.close();

	EXPECT_EQ(ClockError::SUCCESS, sock2.listen(10262, 1, true, [](TcpSocket *, ClockError) {}));

	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, accept) {
	TcpSocket sock1, sock2;
	int a = 0;
	sock1.listen(12345, 1, false, [&a](TcpSocket * sock, ClockError) mutable
		{
			std::unique_lock<std::mutex> ul(connectionLock);
			_socketList.push_back(sock);
			a = 1;
			conditionVariable.notify_one();
	});
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		sock2.connect("127.0.0.1", 12345, 500);
		conditionVariable.wait(ul);
	}
	EXPECT_EQ(1, a);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, write) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	sock1.listen(12345, 1, false, [v](TcpSocket * sock, ClockError)
		{
			_socketList.push_back(sock);
			sock->write(v);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::vector<uint8_t> v2;
	sock2.read(v2);
	EXPECT_EQ(v, v2);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writeMultiple) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1 = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	std::vector<uint8_t> v2 = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11};
	std::vector<uint8_t> vSum = v1;
	vSum.insert(vSum.end(), v2.begin(), v2.end());

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock, ClockError)
		{
			_socketList.push_back(sock);
			sock->write(v1);
			std::unique_lock<std::mutex> ul(connectionLock);
			sock->write(v2);
			conditionVariable.notify_one();
		});

	std::vector<uint8_t> v3;
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		sock2.connect("127.0.0.1", 12345, 500);
		conditionVariable.wait(ul);
		sock2.read(v3);
	}
	EXPECT_EQ(vSum, v3);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, connectDouble) {
	TcpSocket sock1, sock2, sock3;
	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		std::unique_lock<std::mutex> ul(connectionLock);
		_socketList.push_back(sock);
		conditionVariable.notify_one();
	}));
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 100));
		conditionVariable.wait(ul);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	ClockError e = sock3.connect("127.0.0.1", 12345, 100);
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, e);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, e);
#endif
	sock1.close();
	sock2.close();
	sock3.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writePacket) {
	TcpSocket sock1, sock2;
	std::string v = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	sock1.listen(12345, 1, false, [v](TcpSocket * sock, ClockError)
		{
			_socketList.push_back(sock);
			sock->writePacket(&v[0], v.length());
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::string v2;
	sock2.receivePacket(v2);
	EXPECT_EQ(v, v2);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writePacketMultiple) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1 = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	std::vector<uint8_t> v2 = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11};

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock, ClockError)
		{
			_socketList.push_back(sock);
			sock->writePacket(v1);
			sock->writePacket(v2);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::vector<uint8_t> v3, v4;
	sock2.receivePacket(v3);
	EXPECT_EQ(v1, v3);
	sock2.receivePacket(v4);
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writePacketMultipleSwapped) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1 = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	std::vector<uint8_t> v2 = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11};

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock, ClockError) {
		std::unique_lock<std::mutex> ul(connectionLock);
		_socketList.push_back(sock);
		std::vector<uint8_t> v3, v4;
		sock->receivePacket(v3);
		sock->receivePacket(v4);
		EXPECT_EQ(v1, v3);
		EXPECT_EQ(v2, v4);
		sock->close();
		conditionVariable.notify_one();
	});
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		sock2.connect("127.0.0.1", 12345, 500);

		sock2.writePacket(v1);
		sock2.writePacket(v2);
		conditionVariable.wait(ul);
	}

	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writeMass) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1(100000, 'a');
	std::vector<uint8_t> v2(100000, 'b');

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
		sock->writePacket(v1);
		sock->writePacket(v2);
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::vector<uint8_t> v3, v4;
	sock2.receivePacket(v3);
	EXPECT_EQ(v1, v3);
	sock2.receivePacket(v4);
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writeMass2) {
	const int NUM_RUNS = 10000;
	const size_t VEC_SIZE = 10000;

	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1(VEC_SIZE, 'a');
	std::vector<uint8_t> v2(VEC_SIZE, 'b');
	std::vector<uint8_t> v1T = v1, v2T = v2;

	sock1.listen(12345, 1, false, [&v1, &v2, NUM_RUNS, VEC_SIZE](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
		std::vector<uint8_t> v1L = v1, v2L = v2;

		for (int i = 0; i < NUM_RUNS; ++i) {
			for (size_t j = 0; j < VEC_SIZE; ++j) {
				v1L[j]++;
				v2L[j]++;
			}
			EXPECT_EQ(ClockError::SUCCESS, sock->writePacketAsync(v1L));
			EXPECT_EQ(ClockError::SUCCESS, sock->writePacketAsync(v2L));
		}
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::vector<uint8_t> v3, v4;
	for (int i = 0; i < NUM_RUNS; ++i) {
		for (size_t j = 0; j < VEC_SIZE; ++j) {
			v1T[j]++;
			v2T[j]++;
		}
		EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(v3));
		EXPECT_EQ(v1T, v3);
		EXPECT_EQ(ClockError::SUCCESS, sock2.receivePacket(v4));
		EXPECT_EQ(v2T, v4);
	}
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, receiveCallback) {
	receivedCounter = 0;
	TcpSocket sock1, sock2;

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
		sock->receiveCallback(std::bind(receiveMessage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	});
	sock2.connect("127.0.0.1", 12345, 500);

	for (std::string s : messages) {
		EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket(s.c_str(), s.length()));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}

	_socketList.clear();
}

int called = 0;

TEST(TcpSocket, receiveCallbackRemove) {
	called = 0;
	TcpSocket sock1, sock2;

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		sock->receiveCallback([](const std::vector<uint8_t> &, TcpSocket * so, ClockError error) {
			std::unique_lock<std::mutex> ul(connectionLock);
			called++;
			if (error != ClockError::SUCCESS) {
				_socketList.push_back(so);
				EXPECT_EQ(2, called);
			} else {
				EXPECT_EQ(1, called);
			}
			conditionVariable.notify_one();
		});
	});
	sock2.connect("127.0.0.1", 12345, 500);


	std::string s = "Some messsage!";

	{
		std::unique_lock<std::mutex> ul(connectionLock);
		EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket(s.c_str(), s.length()));
		conditionVariable.wait(ul);
	}

	{
		std::unique_lock<std::mutex> ul(connectionLock);
		sock2.close();
		conditionVariable.wait(ul);
	}

	EXPECT_EQ(2, called);

	sock1.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

/**
 * stops a blocking read and tests whether this works or not
 */
TEST(TcpSocket, stopRead) {
	TcpSocket sock1, sock2;

	called = 0;

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::thread([&sock2]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			sock2.close();
		}).detach();
	std::string buffer;
	sock2.receivePacket(buffer);

	sock1.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

/**
 * stops a blocking read started in own thread and tests whether this works or not
 */
TEST(TcpSocket, stopReadAsync) {
	TcpSocket sock1, sock2;

	called = 0;

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
		sock->close();
	});
	sock2.connect("127.0.0.1", 12345, 500);
	sock2.receiveCallback([](const std::vector<uint8_t> &, TcpSocket *, ClockError error) {
			if (error != ClockError::SUCCESS) {
				called = 1;
			}
		});

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	sock2.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	EXPECT_EQ(1, called);

	sock1.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

/**
 * test whether a second socket creation after deletion of all sockets works
 */
TEST(TcpSocket, createSocketAfterDeletion) {
	{
		TcpSocket sock1, sock2;

		called = 0;

		sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
			_socketList.push_back(sock);
			sock->close();
		});
		EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));
		sock2.receiveCallback([](const std::vector<uint8_t> &, TcpSocket *, ClockError error) {
				if (error != ClockError::SUCCESS) {
					called = 1;
				}
			});

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		sock2.close();

		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		EXPECT_EQ(1, called);

		sock1.close();

		for (TcpSocket * sock : _socketList) {
			delete sock;
		}

		_socketList.clear();
	}
	{
		TcpSocket sock1, sock2;

		called = 0;

		sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
			_socketList.push_back(sock);
			sock->close();
		});

		EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));
		sock2.receiveCallback([](const std::vector<uint8_t> &, TcpSocket *, ClockError error) {
				if (error != ClockError::SUCCESS) {
					called = 1;
				}
			});

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		sock2.close();

		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		EXPECT_EQ(1, called);

		sock1.close();

		for (TcpSocket * sock : _socketList) {
			delete sock;
		}
		_socketList.clear();
	}
}

/**
 * tests multiple sending using asyn writePacket
 */
TEST(TcpSocket, writePacketAsyncMultiple) {
	TcpSocket sock1, sock2;

	called = 0;
	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
		for (int i = 0; i < 5000; i++) {
			sock->writePacketAsync(std::vector<uint8_t>({ 0x0, 0x1, 0x2, 0x3, 0x4, 0x5 }));
		}
		for (int i = 0; i < 5000; i++) {
			sock->writePacketAsync(std::vector<uint8_t>({ 0x5, 0x4, 0x3, 0x2, 0x1, 0x0 }));
		}
	});
	{
		std::unique_lock<std::mutex> l(connectionLock);
		EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));
		sock2.receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket *, ClockError error) {
			if (error != ClockError::SUCCESS) {
			} else {
				if (called < 5000) {
					EXPECT_EQ(std::vector<uint8_t>({ 0x0, 0x1, 0x2, 0x3, 0x4, 0x5 }), msg);
				} else {
					EXPECT_EQ(std::vector<uint8_t>({ 0x5, 0x4, 0x3, 0x2, 0x1, 0x0 }), msg);
				}
				called++;
				if (called == 10000) {
					std::unique_lock<std::mutex> ul(connectionLock);
					conditionVariable.notify_one();
				}
			}
		});
		conditionVariable.wait(l);
	}

	EXPECT_EQ(10000, called);

	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writeAsyncMultiple) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1 = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1 };
	std::vector<uint8_t> v2 = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11 };
	std::vector<uint8_t> vSum = v1;
	vSum.insert(vSum.end(), v2.begin(), v2.end());

	ClockError err = sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
		sock->writeAsync(v1);
		sock->writeAsync(v2);
	});
	EXPECT_EQ(ClockError::SUCCESS, err);

	EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	std::vector<uint8_t> v3;
	EXPECT_EQ(ClockError::SUCCESS, sock2.read(v3));
	EXPECT_EQ(vSum, v3);
	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

TEST(TcpSocket, writeAsyncMultipleWithFastShutdown) {
	TcpSocket sock1;
	std::vector<uint8_t> v1 = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1 };

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		_socketList.push_back(sock);
	});
	TcpSocket * sock2 = new TcpSocket();
	sock2->connect("127.0.0.1", 12345, 500);

	for (int i = 0; i < 1000; i++) {
		sock2->writePacketAsync(v1);
	}
	sock2->close();
	delete sock2;

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

/**
 * tests sending messages as stream
 */
TEST(TcpSocket, streamOperator) {
	TcpSocket sock1, sock2;

	enum class StreamTestEnum {
		Value0,
		Value1,
		Value2
	};

	sock1.listen(12345, 1, false, [](TcpSocket * sock, ClockError) {
		*sock << 1 << std::string("Hello") << StreamTestEnum::Value0 << StreamTestEnum::Value1 << StreamTestEnum::Value2;
		delete sock;
	});
	EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));

	int i;
	std::string s;
	StreamTestEnum ste1, ste2, ste3;
	sock2 >> i >> s >> ste1 >> ste2 >> ste3;

	EXPECT_EQ(1, i);
	EXPECT_EQ("Hello", s);
	EXPECT_EQ(StreamTestEnum::Value0, ste1);
	EXPECT_EQ(StreamTestEnum::Value1, ste2);
	EXPECT_EQ(StreamTestEnum::Value2, ste3);

	sock1.close();
	sock2.close();
}

/**
 * tests error code of listen callback
 */
TEST(TcpSocket, listenCloseError) {
	TcpSocket sock1, sock2;
	ClockError error = ClockError::SUCCESS;

	sock1.listen(12345, 1, true, [&error](TcpSocket * sock, ClockError err) {
		if (err == ClockError::SUCCESS) {
			_socketList.push_back(sock);
		} else {
			std::unique_lock<std::mutex> ul(connectionLock);
			error = err;
			conditionVariable.notify_one();
		}
	});
	EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));
	{
		std::unique_lock<std::mutex> ul(connectionLock);
		sock1.close();
		conditionVariable.wait(ul);
	}
	EXPECT_NE(ClockError::SUCCESS, error);

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}
	_socketList.clear();
}

/**
 * tests dns lookup
 */
TEST(TcpSocket, dnsLookup) {
	EXPECT_EQ("83.169.44.32", TcpSocket::getHostnameIP("clockwork-origins.de"));
	EXPECT_EQ("127.0.0.1", TcpSocket::getHostnameIP("localhost"));
	EXPECT_EQ("", TcpSocket::getHostnameIP("lcalhst"));
}
