#include "clockUtils/sockets/TcpSocket.h"

#include "gtest/gtest.h"

using namespace clockUtils;
using namespace clockUtils::sockets;

void connectionAccepted(const TcpSocket &) {

}

TEST(TcpSocket, connect) { // tests connect with all possible errors
	TcpSocket ts;
	ClockError e = ts.connect("1", 12345, 1000);

	EXPECT_EQ(ClockError::INVALID_IP, e);
	
	e = ts.connect("127.0.0.1", 0, 1000);

	EXPECT_EQ(ClockError::INVALID_PORT, e);

	e = ts.connect("192.168.255.255", 12345, 1000);

	EXPECT_EQ(ClockError::TIMEOUT, e);

	ts.connect("127.0.0.1", 12345, 1000);

	EXPECT_EQ(ClockError::CONNECTION_FAILED, e);

	TcpSocket server;

	e = server.listen(12345, 1, false, std::bind(connectionAccepted, std::placeholders::_1));

	e = ts.connect("127.0.0.1", 12345, 1000);

	EXPECT_EQ(ClockError::SUCCESS, e);
}

TEST(TcpSocket, listen) { // tests incoming connections: one thread listening on a port and five or something like that joining
}

TEST(TcpSocket, sendRead) { // tests communication between two sockets
}

TEST(TcpSocket, close) { // tests closing of a connection and if it is registered in socket on the other side
}

TEST(TcpSocket, getIP) { // tests IP before and after connection
}

TEST(TcpSocket, getPort) { // tests port before and after connection
}

TEST(TcpSocket, useUnready) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	int a;

	TcpSocket sock1;
	/*EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(static_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer));*/
}
