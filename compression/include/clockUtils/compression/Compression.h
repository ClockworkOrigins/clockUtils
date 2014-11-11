#pragma once

#include <string>

namespace clockUtils {
namespace compression {

	/**
	 * \brief class for string compression and decompression using Algorithm as template parameter
	 */
	template<typename Algorithm>
	class Compression {
	public:
		/**
		 * \brief compresses the given string and returns result
		 */
		std::string compress(const std::string & text) const {
			return Algorithm::compress(text);
		}

		/**
		 * \brief decompresses the given string and returns result
		 */
		std::string decompress(const std::string & text) const {
			return Algorithm::decompress(text);
		}
	};

} /* namespace compression */
} /* namespace clockUtils */
