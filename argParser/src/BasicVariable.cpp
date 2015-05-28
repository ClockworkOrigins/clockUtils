#include "clockUtils/argParser/BasicVariable.h"

#include "clockUtils/argParser/Parser.h"

namespace clockUtils {
namespace argParser {

	BasicVariable::BasicVariable(const std::string & name, const std::string & description) : _name(name), _description(description) {
		clockUtils::argParser::Parser::variableList.push_back(this);
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

	template<>
	bool Variable<std::string>::setValue(const std::string & value) {
		_value = value;
		return true;
	}

	template<>
	bool Variable<char>::setValue(const std::string & value) {
		bool ret = value.size() == 1;
		if (ret) {
			_value = value.at(0);
		}
		return ret;
	}

} /* namespace argParser */
} /* namespace clockUtils */
