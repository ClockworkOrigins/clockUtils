/*
 * clockUtils
 * Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
 *
 * This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "clockUtils/argParser/BasicVariable.h"

#include "clockUtils/argParser/Parser.h"

namespace clockUtils {
namespace argParser {

	BasicVariable::BasicVariable(const std::string & name, const std::string & description) : _name(name), _description(description), _set(false) {
		clockUtils::argParser::Parser::variableList.push_back(this);
	}

	BasicVariable::~BasicVariable() {
		for (size_t i = 0; i < Parser::variableList.size(); i++) {
			if (_name == Parser::variableList[i]->getName()) {
				Parser::variableList.erase(Parser::variableList.begin() + int(i));
				Parser::helpTexts.erase(_name);
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
