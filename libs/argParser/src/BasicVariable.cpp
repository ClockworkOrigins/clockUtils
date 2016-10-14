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

#include <algorithm>

#include "clockUtils/argParser/Parser.h"

namespace clockUtils {
namespace argParser {

	BasicVariable::BasicVariable(const std::string & longname, const std::string & shortname, const std::string & description, bool required, bool multiple) : _set(false), _longname(longname), _shortname(shortname), _description(description), _required(required), _multiple(multiple) {
		clockUtils::argParser::Parser::variableList.push_back(this);
	}

	BasicVariable::~BasicVariable() {
		for (size_t i = 0; i < Parser::variableList.size(); i++) {
			if (_longname == Parser::variableList[i]->getLongname()) {
				Parser::variableList.erase(Parser::variableList.begin() + int(i));
				Parser::helpTexts.erase(_longname);
				break;
			}
		}
	}

	template<>
	bool Variable<std::string>::setValue(const std::string & value) {
		if (!_set) {
			_value = value;
		} else {
			_values.push_back(value);
		}
		return true;
	}

	template<>
	bool Variable<char>::setValue(const std::string & value) {
		bool ret = value.size() == 1;
		if (ret) {
			if (!_set) {
				_value = value.at(0);
			} else {
				_values.push_back(value.at(0));
			}
		}
		return ret;
	}

	template<>
	bool Variable<bool>::setValue(const std::string & value) {
		std::string result = value;
		std::transform(value.begin(), value.end(), result.begin(), ::tolower);
		bool ret = true;
		bool val;
		if (result == "true") {
			val = true;
		} else if (result == "false") {
			val = false;
		} else {
			std::stringstream ss(value);
			ret = !(ss >> (val)).fail() && ss.eof();
		}
		if (!_set) {
			_value = val;
		} else {
			_values.push_back(val);
		}
		return ret;
	}

} /* namespace argParser */
} /* namespace clockUtils */
