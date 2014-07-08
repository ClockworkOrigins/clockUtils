#define _GLIBCXX_USE_NANOSLEEP // needed for sleep_for, see http://stackoverflow.com/questions/4438084/stdthis-threadsleep-for-and-gcc

#include <chrono>
#include <thread>

#include "clockUtils/errors.h"
#include "clockUtils/sockets/TcpSocket.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::sockets;

int connectCounter = 0;
std::vector<std::string> messages = { "Hello", "World!", "This is a super nice message", "111elf!!!" };
std::vector<TcpSocket *> _socketList;

void connectionAccepted(TcpSocket * sock) {
	connectCounter++;
	_socketList.push_back(sock);
}

void connectionAcceptedWrite(TcpSocket * ts) {
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

void receiveMessage(const std::vector<uint8_t> & message, TcpSocket * sock, ClockError error) {
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
	ClockError e = ts.connect("1", 12345, 500);

	EXPECT_EQ(ClockError::INVALID_IP, e);
	e = ts.connect("127.0.0.1", 0, 500);

	EXPECT_EQ(ClockError::INVALID_PORT, e);
	e = ts.connect("192.168.255.255", 12345, 500);

	EXPECT_EQ(ClockError::TIMEOUT, e);
	ts.connect("127.0.0.1", 12345, 500);

	EXPECT_EQ(ClockError::TIMEOUT, e);
	TcpSocket server;

	e = server.listen(12345, 1, true, [](TcpSocket * sock) {
		_socketList.push_back(sock);
	});

	e = ts.connect("127.0.0.1", 12345, 500);

	EXPECT_EQ(ClockError::SUCCESS, e);
	e = ts.connect("127.0.0.1", 12345, 500);

	EXPECT_EQ(ClockError::INVALID_USAGE, e);
	ts.close();

	e = ts.connect("127.0.0.1", 12345, 500);

	EXPECT_EQ(ClockError::SUCCESS, e);
	server.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}

	_socketList.clear();
}

TEST(TcpSocket, listen) { // tests incoming connections: one thread listening on a port and five or something like that joining
	TcpSocket server1;

	ClockError e = server1.listen(0, 10, true, std::bind(connectionAccepted, std::placeholders::_1));

	EXPECT_EQ(ClockError::INVALID_PORT, e);

	e = server1.listen(12345, 10, true, std::bind(connectionAccepted, std::placeholders::_1));

	EXPECT_EQ(ClockError::SUCCESS, e);

	TcpSocket server2;

	std::this_thread::sleep_for(std::chrono::milliseconds(600));

	e = server2.listen(12345, 10, true, std::bind(connectionAccepted, std::placeholders::_1));

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::ADDRESS_INUSE, e);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::SUCCESS, e);
#endif

	server2.close();

	connectCounter = 0;

	TcpSocket client1;
	e = client1.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(600));
	EXPECT_EQ(ClockError::SUCCESS, e);
	EXPECT_EQ(1, connectCounter);

	TcpSocket client2;
	e = client2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(600));
	EXPECT_EQ(ClockError::SUCCESS, e);
	EXPECT_EQ(2, connectCounter);

	TcpSocket client3;
	e = client3.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(600));
	EXPECT_EQ(ClockError::SUCCESS, e);
	EXPECT_EQ(3, connectCounter);

	e = server2.listen(12346, 10, false, std::bind(connectionAccepted, std::placeholders::_1));

	EXPECT_EQ(ClockError::SUCCESS, e);

	TcpSocket client4;
	e = client4.connect("127.0.0.1", 12346, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(600));
	EXPECT_EQ(ClockError::SUCCESS, e);
	EXPECT_EQ(4, connectCounter);

	TcpSocket client5;
	e = client5.connect("127.0.0.1", 12346, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(600));
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, e);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, e);
#endif
	EXPECT_EQ(4, connectCounter);

	client1.close();
	client2.close();
	client3.close();
	client4.close();
	client5.close();
	server1.close();
	server2.close();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	TcpSocket client6;
	e = client6.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(600));
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, e);
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, e);
#endif
	EXPECT_EQ(4, connectCounter);

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

	ClockError e = server.listen(12345, 10, false, std::bind(connectionAcceptedWrite, std::placeholders::_1));

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

TEST(TcpSocket, getIP) { // tests IP before and after connection
	TcpSocket ts;
	std::string s2 = ts.getPublicIP();
	std::string s3 = ts.getRemoteIP();

	EXPECT_EQ(0, s2.length());
	EXPECT_EQ(0, s3.length());

	TcpSocket server;
	server.listen(12345, 1, false, [](TcpSocket * client) {
		std::string s4 = client->getRemoteIP();
		std::string s5 = client->getPublicIP();

		EXPECT_NE(0, s4.length());
		EXPECT_NE(0, s5.length());
		EXPECT_EQ("127.0.0.1", s4);
		_socketList.push_back(client);
	});

	ts.connect("127.0.0.1", 12345, 500);
	s2 = ts.getPublicIP();
	s3 = ts.getRemoteIP();

	EXPECT_NE(0, s2.length());
	EXPECT_NE(0, s3.length());

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
	server.listen(12345, 1, false, [](TcpSocket * client) {
		uint16_t s3 = client->getRemotePort();
		uint16_t s4 = client->getLocalPort();

		EXPECT_NE(0, s3);
		EXPECT_EQ(12345, s4);

		client->close();
		delete client;
	});

	ts.connect("127.0.0.1", 12345, 500);
	s = ts.getLocalPort();
	s2 = ts.getRemotePort();

	EXPECT_NE(0, s);
	EXPECT_EQ(12345, s2);

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

	EXPECT_EQ(ClockError::INVALID_PORT, sock1.connect("127.0.0.1", 0, 500));

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, sock1.connect("127.0.0.1", 1, 500));
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, sock1.connect("127.0.0.1", 1, 500));
#endif
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.", 1, 500));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0", 1, 500));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("-12.0.0.1", 1, 500));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("12.-3.0.1", 1, 500));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.1.9", 1, 500));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("256.0.0.1", 1, 500));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.1234.0.1", 1, 500));

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}

	_socketList.clear();
}

TEST(TcpSocket, invalidUse) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;

	TcpSocket sock1;
	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(1025, 1, true, [](TcpSocket * sock) {
		_socketList.push_back(sock);
	}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.listen(1025, 1, true, [](TcpSocket *){}));
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

	sock1.listen(12345, 1, false, [](TcpSocket * sock)
		{
		_socketList.push_back(sock);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	EXPECT_EQ(12345, sock2.getRemotePort());
	EXPECT_EQ(_socketList[0]->getLocalPort(), sock2.getRemotePort());
	EXPECT_EQ(sock2.getLocalPort(), _socketList[0]->getRemotePort());

	sock1.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}

	_socketList.clear();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(1025, 1, true, [](TcpSocket * sock){}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.listen(1025, 1, true, [](TcpSocket * sock){}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.connect("127.0.0.1", 1026, 500));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(str));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(str));

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	sock2.writePacket(buffer);
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.read(buffer));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.read(str));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.receivePacket(str));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_CONNECTED, sock2.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.listen(1026, 1, true, [](TcpSocket * sock) {}));

	sock2.close();

	EXPECT_EQ(ClockError::SUCCESS, sock2.listen(1026, 1, true, [](TcpSocket * sock) {}));

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
	sock1.listen(12345, 1, false, [&a](TcpSocket * sock) mutable
		{
			a = 1;
			_socketList.push_back(sock);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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
	sock1.listen(12345, 1, false, [v](TcpSocket * sock)
		{
			sock->write(v);
			_socketList.push_back(sock);
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

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock)
		{
			sock->write(v1);
			sock->write(v2);
			_socketList.push_back(sock);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::vector<uint8_t> v3;
	sock2.read(v3);
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
	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(12345, 1, false, [](TcpSocket * sock) {
		_socketList.push_back(sock);
	}));
	EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 500));
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	EXPECT_EQ(ClockError::CONNECTION_FAILED, sock3.connect("127.0.0.1", 12345, 500));
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	EXPECT_EQ(ClockError::TIMEOUT, sock3.connect("127.0.0.1", 12345, 500));
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
	sock1.listen(12345, 1, false, [v](TcpSocket * sock)
		{
			sock->writePacket(&v[0], v.length());
			_socketList.push_back(sock);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock)
		{
			sock->writePacket(v1);
			sock->writePacket(v2);
			_socketList.push_back(sock);
		});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock)
		{
			std::vector<uint8_t> v3, v4;
			sock->receivePacket(v3);
			sock->receivePacket(v4);
			EXPECT_EQ(v1, v3);
			EXPECT_EQ(v2, v4);
			sock->close();
			sock->close();
			delete sock;
		});
	sock2.connect("127.0.0.1", 12345, 500);

	sock2.writePacket(v1);
	sock2.writePacket(v2);

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

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

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket * sock) {
		sock->writePacket(v1);
		sock->writePacket(v2);
		_socketList.push_back(sock);
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

	sock1.listen(12345, 1, false, [&v1, &v2, NUM_RUNS, VEC_SIZE](TcpSocket * sock) {
		std::vector<uint8_t> v1L = v1, v2L = v2;

		for (int i = 0; i < NUM_RUNS; ++i) {
			for (size_t j = 0; j < VEC_SIZE; ++j) {
				v1L[j]++;
				v2L[j]++;
			}
			sock->writePacketAsync(v1L);
			sock->writePacketAsync(v2L);
		}
		_socketList.push_back(sock);
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::vector<uint8_t> v3, v4;
	for (int i = 0; i < NUM_RUNS; ++i) {
		for (size_t j = 0; j < VEC_SIZE; ++j) {
			v1T[j]++;
			v2T[j]++;
		}
		sock2.receivePacket(v3);
		EXPECT_EQ(v1T, v3);
		sock2.receivePacket(v4);
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
	TcpSocket sock1, sock2;

	sock1.listen(12345, 1, false, [](TcpSocket * sock) {
		sock->receiveCallback(std::bind(receiveMessage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		_socketList.push_back(sock);
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	for (std::string s : messages) {
		EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket(s.c_str(), s.length()));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	sock1.close();
	sock2.close();

	for (TcpSocket * sock : _socketList) {
		delete sock;
	}

	_socketList.clear();
}

int called = 0;

TEST(TcpSocket, receiveCallbackRemove) {
	TcpSocket sock1, sock2;

	sock1.listen(12345, 1, false, [](TcpSocket * sock) {
		sock->receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
			called++;
			if (error != ClockError::SUCCESS) {
				so->close();
				delete so;
				EXPECT_EQ(2, called);
			} else {
				EXPECT_EQ(1, called);
			}
		});
	});
	sock2.connect("127.0.0.1", 12345, 500);


	std::string s = "Some messsage!";

	EXPECT_EQ(ClockError::SUCCESS, sock2.writePacket(s.c_str(), s.length()));

	sock2.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	EXPECT_EQ(2, called);

	sock1.close();
}

/**
 * stops a blocking read and tests whether this works or not
 */
TEST(TcpSocket, stopRead) {
	TcpSocket sock1, sock2;

	called = 0;

	sock1.listen(12345, 1, false, [](TcpSocket * sock) {
		sock->receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
			if (error != ClockError::SUCCESS) {
				so->close();
				delete so;
			}
		});
	});
	sock2.connect("127.0.0.1", 12345, 500);

	std::thread thrd([&sock2](){
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			sock2.close();
		});
	thrd.detach();
	std::string buffer;
	sock2.receivePacket(buffer);

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	sock1.close();
}

/**
 * stops a blocking read started in own thread and tests whether this works or not
 */
TEST(TcpSocket, stopReadAsync) {
	TcpSocket sock1, sock2;

	called = 0;

	sock1.listen(12345, 1, false, [](TcpSocket * sock) {
		sock->receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
			if (error != ClockError::SUCCESS) {
				so->close();
				delete so;
			}
		});
	});
	sock2.connect("127.0.0.1", 12345, 500);
	sock2.receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
			if (error != ClockError::SUCCESS) {
				called = 1;
			}
		});

	sock2.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	EXPECT_EQ(1, called);

	sock1.close();
}

/**
 * test whether a second socket creation after deletion of all sockets works
 */
TEST(TcpSocket, createSocketAfterDeletion) {
	{
		TcpSocket sock1, sock2;

		called = 0;

		sock1.listen(12345, 1, false, [](TcpSocket * sock) {
			sock->receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
				if (error != ClockError::SUCCESS) {
					so->close();
					delete so;
				}
			});
		});
		sock2.connect("127.0.0.1", 12345, 500);
		sock2.receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
				if (error != ClockError::SUCCESS) {
					called = 1;
				}
			});

		sock2.close();

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		EXPECT_EQ(1, called);

		sock1.close();
	}
	{
		TcpSocket sock1, sock2;

		called = 0;

		sock1.listen(12345, 1, false, [](TcpSocket * sock) {
			sock->receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
				if (error != ClockError::SUCCESS) {
					so->close();
					delete so;
				}
			});
		});
		sock2.connect("127.0.0.1", 12345, 500);
		sock2.receiveCallback([](const std::vector<uint8_t> & msg, TcpSocket * so, ClockError error) {
				if (error != ClockError::SUCCESS) {
					called = 1;
				}
			});

		sock2.close();

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		EXPECT_EQ(1, called);

		sock1.close();
	}
}
