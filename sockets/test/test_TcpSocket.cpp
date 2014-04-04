#include "clockUtils/sockets/TcpSocket.h"

#include "gtest/gtest.h"

#define _GLIBCXX_USE_NANOSLEEP // needed for sleep_for, see http://stackoverflow.com/questions/4438084/stdthis-threadsleep-for-and-gcc

#include <chrono>
#include <thread>

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
	std::string str;

	TcpSocket sock1;
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.read(str));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::NOT_READY, sock1.receivePacket(str));
}

TEST(TcpSocket, invalidParameters) {
	TcpSocket sock1;
	TcpSocket::acceptCallback acb;
	EXPECT_EQ(ClockError::INVALID_PORT, sock1.listen(0, 1, true, acb));

	EXPECT_EQ(ClockError::INVALID_PORT, sock1.connect("127.0.0.1", 0, 1000));

	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.1", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("-12.0.0.1", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("12.-3.0.1", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.0.0.1.9", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("256.0.0.1", 1, 1000));
	EXPECT_EQ(ClockError::INVALID_IP, sock1.connect("127.1234.0.1", 1, 1000));
}

TEST(TcpSocket, invalidUse) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;

	TcpSocket sock1;
	TcpSocket::acceptCallback acb;
	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(1025, 1, true, acb));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.listen(1025, 1, true, acb));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.connect("127.0.0.1", 1026, 1000));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.read(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.read(str));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.receivePacket(str));
}

TEST(TcpSocket, getterUnused) {
	TcpSocket sock1;
	EXPECT_EQ("", sock1.getRemoteIP());
	EXPECT_EQ(0, sock1.getLocalPort());
	EXPECT_EQ(0, sock1.getRemotePort());
}

TEST(TcpSocket, closeFails) {
	TcpSocket sock1;
	sock1.close();
	sock1.close();
}

TEST(TcpSocket, connectOnly) {
	std::vector<uint8_t> buffer = { 0x10, 0x11, 0x12 };
	std::string str;
	TcpSocket sock1, sock2;

	sock1.listen(12345, 1, false, [](TcpSocket &){});
	sock2.connect("127.0.0.1", 12345, 1000);
	
	EXPECT_EQ(12345, sock2.getRemotePort());
	EXPECT_EQ(sock1.getLocalPort(), sock2.getRemotePort());
	EXPECT_EQ(sock2.getLocalPort(), sock1.getRemotePort());
	
	sock1.close();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(1025, 1, true, [](TcpSocket & sock){}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.listen(1025, 1, true, [](TcpSocket & sock){}));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.connect("127.0.0.1", 1026, 1000));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.writePacket(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.read(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.read(str));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.receivePacket(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock1.receivePacket(str));

	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.writePacket(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.writePacket(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.write(reinterpret_cast<char *>(&buffer[0]), buffer.size()));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.read(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.read(str));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.receivePacket(buffer));
	EXPECT_EQ(ClockError::INVALID_USAGE, sock2.receivePacket(str));
	EXPECT_EQ(ClockError::SUCCESS, sock2.listen(1026, 1, true, [](TcpSocket & sock){}));
	
	sock1.close();
	sock2.close();
}

TEST(TcpSocket, write) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	sock1.listen(12345, 1, false, [v](TcpSocket & sock)
		{
			sock.write(v);
		});
	sock2.connect("127.0.0.1", 12345, 1000);
	
	std::vector<uint8_t> v2;
	sock2.read(v2);
	EXPECT_EQ(v, v2);
	sock1.close();
	sock2.close();
}

TEST(TcpSocket, write2) {
	TcpSocket sock1, sock2;
	std::string v = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	sock1.listen(12345, 1, false, [v](TcpSocket & sock)
		{
			sock.write(&v[0], v.length());
		});
	sock2.connect("127.0.0.1", 12345, 1000);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	std::string v2;
	sock2.read(v2);
	EXPECT_EQ(v, v2);
	sock1.close();
	sock2.close();
}

TEST(TcpSocket, writeMultiple) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1 = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	std::vector<uint8_t> v2 = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11};
	std::vector<uint8_t> vSum = v1;
	vSum.insert(vSum.end(), v2.begin(), v2.end());

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket & sock)
		{
			sock.write(v1);
			sock.write(v2);
		});
	sock2.connect("127.0.0.1", 12345, 1000);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::vector<uint8_t> v3;
	sock2.read(v3);
	EXPECT_EQ(vSum, v3);
	sock1.close();
	sock2.close();
}

TEST(TcpSocket, connectDouble) {
	TcpSocket sock1, sock2, sock3;
	EXPECT_EQ(ClockError::SUCCESS, sock1.listen(12345, 1, false, [](TcpSocket &){}));
	EXPECT_EQ(ClockError::SUCCESS, sock2.connect("127.0.0.1", 12345, 1000));
	EXPECT_EQ(ClockError::CONNECTION_FAILED, sock3.connect("127.0.0.1", 12345, 1000));
	sock1.close();
	sock2.close();
	sock3.close();
}

TEST(TcpSocket, writePacket) {
	TcpSocket sock1, sock2;
	std::string v = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	sock1.listen(12345, 1, false, [v](TcpSocket & sock)
		{
			sock.writePacket(&v[0], v.length());
		});
	sock2.connect("127.0.0.1", 12345, 1000);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	std::string v2;
	sock2.receivePacket(v2);
	EXPECT_EQ(v, v2);
	sock1.close();
	sock2.close();
}

TEST(TcpSocket, writePacketMultiple) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1 = {0x1, 0x2, 0x3, 0x4, 0x5, 0x0, 0x5, 0x4, 0x3, 0x2, 0x1};
	std::vector<uint8_t> v2 = {0x11, 0x12, 0x13, 0x14, 0x15, 0x0, 0x15, 0x14, 0x13, 0x12, 0x11};

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket & sock)
		{
			sock.writePacket(v1);
			sock.writePacket(v2);
		});
	sock2.connect("127.0.0.1", 12345, 1000);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::vector<uint8_t> v3, v4;
	sock2.receivePacket(v3);
	sock2.receivePacket(v4);
	EXPECT_EQ(v1, v3);
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();
}

TEST(TcpSocket, writeMass) {
	TcpSocket sock1, sock2;
	std::vector<uint8_t> v1(10000, 'a');
	std::vector<uint8_t> v2(10000, 'b');

	sock1.listen(12345, 1, false, [v1, v2](TcpSocket & sock)
		{
			sock.writePacket(v1);
			sock.writePacket(v2);
		});
	sock2.connect("127.0.0.1", 12345, 1000);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::vector<uint8_t> v3, v4;
	sock2.receivePacket(v3);
	sock2.receivePacket(v4);
	EXPECT_EQ(v1, v3);
	EXPECT_EQ(v2, v4);
	sock1.close();
	sock2.close();
}
