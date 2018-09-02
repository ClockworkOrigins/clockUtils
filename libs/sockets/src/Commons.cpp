#include "clockUtils/sockets/Commons.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <WS2tcpip.h>
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_LINUX
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <cstring>
#endif

namespace clockUtils {
namespace sockets {
namespace {

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	class WSAHelper {
	public:
		WSAHelper() {
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 2), &wsa);
		}

		~WSAHelper() {
			WSACleanup();
		}
	};
#endif

}

	IPv4 resolveHostname(const std::string & hn) {
#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
		static WSAHelper wsa;
#endif
		struct addrinfo hints, * servinfo;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
		hints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(hn.c_str(), nullptr, nullptr, &servinfo) != 0) {
			return NO_IP;
		}

		static IPv4 localHost = convertIP("127.0.0.1");

		IPv4 ip = NO_IP;
		// loop through all the results and connect to the first we can
		for (struct addrinfo * p = servinfo; p != nullptr; p = p->ai_next) {
			struct sockaddr_in * h = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
			ip = h->sin_addr.s_addr;

			if (ip != NO_IP && ip != 0 && ip != localHost) {
				break; // FIXME: we should try the first one first, then the second etc. But the first fails with localhost
			}
		}

		freeaddrinfo(servinfo); // all done with this structure
		return ip;
	}

	IPv4 convertIP(const std::string & ip) {
		// FIXME: this function is deprecated because the error value INADDR_NONE (-1) is a valid ip
		return inet_addr(ip.c_str());
	}

	std::string convertIP(const IPv4 & ip) {
		return inet_ntoa(*reinterpret_cast<const in_addr *>(&ip));
	}

} /* namespace sockets */
} /* namespace clockUtils */
