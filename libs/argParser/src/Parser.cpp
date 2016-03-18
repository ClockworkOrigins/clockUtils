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

#include "clockUtils/argParser/Parser.h"

#include <cstring>

#include "clockUtils/errors.h"
#include "clockUtils/argParser/BasicVariable.h"

namespace clockUtils {
namespace argParser {

	std::vector<BasicVariable *> Parser::variableList = std::vector<BasicVariable *>();
	std::string Parser::error = std::string();
	bool Parser::errorOnFlag = true;
	bool Parser::help = false;
	std::map<std::string, std::string> Parser::helpTexts = std::map<std::string, std::string>();
	std::vector<std::string> * Parser::arguments = nullptr;

	ClockError Parser::parseArguments(const char ** argv, int argc) {
		// reset variables to default value
		for (BasicVariable * bv : variableList) {
			bv->resetToDefault();
		}
		error = "";
		help = false;
		ClockError result = ClockError::SUCCESS;
		std::map<std::string, bool> parsed;
		while (argc > 0) {
			if (argv[0][0] == '-' && argv[0][1] != '\0') {
				std::string name(&argv[0][1]);

				bool found = false;

				for (BasicVariable * bv : variableList) {
					std::string longname = "-" + bv->getLongname();
					std::string shortname = bv->getShortname();
					if ((name.find(longname) == 0 && longname.length() == name.length()) || (name.find(shortname) == 0 && shortname.length() == name.length())) {
						found = true;
						if (bv->isBool()) { // boolean is special case: variable name and parsed name are the same, so it can be "-b" or "-b true", which are both valid
							if (argc > 1 && argv[1][0] != '-') { // case "-b true"
								if (!bv->setValue(argv[1])) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								if (parsed.find(bv->getLongname()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getLongname()] = true;
								argc--;
								argv++;
							} else { // case "-b"
								bv->setValue("1");
								if (parsed.find(bv->getLongname()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getLongname()] = true;
							}
						} else {
							if (argc == 1) {
								error = name + std::string(" requires a value: -") + name + std::string(" <value> or -") + name + std::string("<value> or -") + name + std::string("=<value>");
								arguments = nullptr;
								return ClockError::INVALID_USAGE;
							} else {
								if (!bv->setValue(argv[1])) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								if (parsed.find(bv->getLongname()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getLongname()] = true;
								argc--;
								argv++;
							}
						}
						break;
					}
				}

				if (!found) {
					if (name == "-help" || name == "h") {
						help = true;
					} else if (errorOnFlag) {
						error = std::string("argument -") + name + std::string(" not registered!");
						arguments = nullptr;
						return ClockError::INVALID_USAGE;
					} else {
						if (arguments == nullptr) {
							arguments = nullptr;
							return ClockError::INVALID_USAGE;
						} else {
							arguments->push_back(argv[0]);
						}
					}
				}
			} else if (std::strlen(argv[0]) > 0) {
				if (arguments == nullptr) {
					arguments = nullptr;
					return ClockError::INVALID_USAGE;
				} else {
					arguments->push_back(argv[0]);
				}
			} else if (std::strlen(argv[0]) == 0) {
				error = std::string("parsing empty string not possible!");
				return ClockError::INVALID_USAGE;
			}

			argv++;
			argc--;
		}

		arguments = nullptr;
		return result;
	}

	std::string Parser::getLastParserError() {
		return error;
	}

	std::string Parser::getHelpText() {
		std::string text;
		for (auto it = helpTexts.begin(); it != helpTexts.end();) {
			text += it->second;
			it++;
			if (it != helpTexts.end()) {
				text += "\n";
			}
		}
		return text;
	}

} /* namespace argParser */
} /* namespace clockUtils */
