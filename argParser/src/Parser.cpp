/**
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

#include <iostream>
#include <map>

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
		error = "";
		help = false;
		ClockError result = ClockError::SUCCESS;
		std::map<std::string, bool> parsed;
		while (argc > 0) {
			if (argv[0][0] == '-' && argv[0][1] != '\0') {
				std::string name(&argv[0][1]);

				bool found = false;

				for (BasicVariable * bv : variableList) {
					if (name.find(bv->getName()) == 0) {
						found = true;
						if (bv->isBool()) {
							if (bv->getName().length() == name.length()) {
								bv->setValue("1");
								if (parsed.find(bv->getName()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getName()] = true;
							} else {
								found = false;
								continue;
							}
						} else {
							if (name.length() > bv->getName().length()) {
								size_t startIndex = bv->getName().length();
								if (name.at(startIndex) == '=') {
									startIndex++;
								}
								if (!bv->setValue(name.substr(startIndex, name.length()))) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
							} else if (argc == 1) {
								error = name + std::string(" requires a value: -") + name + std::string(" <value> or -") + name + std::string("<value> or -") + name + std::string("=<value>");
								arguments = nullptr;
								return ClockError::INVALID_USAGE;
							} else {
								if (!bv->setValue(argv[1])) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								if (parsed.find(bv->getName()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getName()] = true;
								argc--;
								argv++;
							}
						}
						break;
					}
				}

				if (!found) {
					if (name == "-help") {
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
			} else {
				if (arguments == nullptr) {
					arguments = nullptr;
					return ClockError::INVALID_USAGE;
				} else {
					arguments->push_back(argv[0]);
				}
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
