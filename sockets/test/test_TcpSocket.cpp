//#include "clockUtils/sockets/TcpSocket.h"

#include "gtest/gtest.h"

TEST(TcpSocket, connect) { // tests connect with all possible errors
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
	EXPECT_EQ(ClockErros::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockErros::NOT_READY, sock1.writePacket(static_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockErros::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockErros::NOT_READY, sock1.receivePacket(buffer));
}

s
