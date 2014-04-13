#include "clockUtils/argparser/BasicVariable.h"

#include "clockUtils/argparser/Parser.h"

namespace clockUtils {
namespace argparser {

	BasicVariable::BasicVariable(const std::string & name, const std::string & description) : _name(name), _description(description) {
		clockUtils::argparser::Parser::variableList.push_back(this);
	}

	BasicVariable::~BasicVariable() {
		for (size_t i = 0; i < Parser::variableList.size(); i++) {
			if (_name == Parser::variableList[i]->getName()) {
				Parser::variableList.erase(Parser::variableList.begin() + int(i));
				break;
			}
		}
	}

	template<>
	bool Variable<bool>::isBool() const {
		return true;
	}

} /* namespace argparser */
} /* namespace clockUtils */
