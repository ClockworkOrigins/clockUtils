#include "clockUtils/argparser/BasicVariable.h"

namespace clockUtils {
namespace argparser {

	BasicVariable::BasicVariable(const std::string & name, const std::string & description, const std::string & type) : _name(name), _description(description), _type(type) {
	}

} /* namespace argparser */
} /* namespace clockUtils */
