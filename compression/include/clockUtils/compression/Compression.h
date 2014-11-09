#pragma once

#include <string>

namespace clockUtils {
namespace compression {

	template<typename Algorithm>
	class Compression {
	public:
		std::string compress(const std::string & text) const {
			return Algorithm::compress(text);
		}

		std::string decompress(const std::string & text) const {
			return Algorithm::decompress(text);
		}
	};

} /* namespace compression */
} /* namespace clockUtils */
