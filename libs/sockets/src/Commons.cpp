#include "clockUtils/sockets/Commons.h"

#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include<iostream>
namespace clockUtils {
namespace sockets {

	IPv4 resolveHostname(const std::string & url) {
		struct addrinfo hints, * servinfo, * p;
		struct sockaddr_in * h;
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo(url.c_str(), NULL, NULL, &servinfo)) != 0) {
			return NO_IP;
		}

		uint32_t ip;
		// loop through all the results and connect to the first we can
		for (p = servinfo; p != NULL; p = p->ai_next) {
			h = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
			ip = h->sin_addr.s_addr;
			break; // FIXME: we should try the first one first, then the second etc. But the first fails with localhost
		}

		freeaddrinfo(servinfo); // all done with this structure
		return ip;
	}

	IPv4 convertIP(const std::string & ip) {
		in_addr res;
		int e = inet_aton(ip.c_str(), &res);
		if (e == 0) {
			return NO_IP;
		}
		return res.s_addr;
	}

	std::string convertIP(const IPv4 & ip) {
		return inet_ntoa(in_addr{ip});
	}

} /* namespace sockets */
} /* namespace clockUtils */
