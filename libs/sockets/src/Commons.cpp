#include "clockUtils/sockets/Commons.h"

#if CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_WIN32
	#include <ws2tcpip.h>
#elif CLOCKUTILS_PLATFORM == CLOCKUTILS_PLATFORM_UNIX
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/types.h>
#endif

namespace clockUtils {
namespace sockets {

	IPv4 resolveHostname(const std::string & url) {
		struct addrinfo hints, * servinfo, * p;
		struct sockaddr_in * h;
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo(url.c_str(), nullptr, nullptr, &servinfo)) != 0) {
			return NO_IP;
		}

		uint32_t ip;
		// loop through all the results and connect to the first we can
		for (p = servinfo; p != nullptr; p = p->ai_next) {
			h = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
			ip = h->sin_addr.s_addr;
			//break; // FIXME: we should try the first one first, then the second etc. But the first fails with localhost
		}

		freeaddrinfo(servinfo); // all done with this structure
		return ip;
	}

	IPv4 convertIP(const std::string & ip) {
		return inet_addr(ip.c_str());
	}

	std::string convertIP(const IPv4 & ip) {
		return inet_ntoa(*reinterpret_cast<const in_addr *>(&ip));
	}

} /* namespace sockets */
} /* namespace clockUtils */
