#pragma once

#include <string>

namespace clockUtils {
namespace compression {

	template<typename Algorithm>
	class Compression {
	public:
		Compression(const std::string & text) : _text(text) {
		}

		std::string compress() const {
			return Algorithm::compress(_text);
		}

		std::string decompress() const {
			return Algorithm::decompress(_text);
		}

	private:
		std::string _text;
	};

} /* namespace compression */
} /* namespace clockUtils */
