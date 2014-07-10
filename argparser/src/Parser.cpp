#include "clockUtils/argparser/Parser.h"

#include <iostream>
#include <map>

#include "clockUtils/errors.h"
#include "clockUtils/argparser/BasicVariable.h"

namespace clockUtils {
namespace argparser {

	std::vector<BasicVariable *> Parser::variableList = std::vector<BasicVariable *>();
	std::string Parser::error = std::string();

	ClockError Parser::parseArguments(const char ** argv, int argc) {
		error = "";
		ClockError result = ClockError::SUCCESS;
		std::map<std::string, bool> parsed;
		while (argc > 0) {
			if (argv[0][0] == '-' && argv[0][1] != '\0') {
				std::string name(&argv[0][1]);

				bool found = false;

				for (BasicVariable * bv : Parser::variableList) {
					if (name.find(bv->getName()) == 0) {
						found = true;
						if (bv->isBool()) {
							if (bv->getName().length() == name.length()) {
								bv->setValue("1");
								if (parsed.find(bv->getName()) != parsed.end()) {
									return ClockError::INVALID_USAGE;
								}
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
									return ClockError::INVALID_USAGE;
								}
							} else if (argc == 1) {
								error = name + std::string(" requires a value: -") + name + std::string(" <value> or -") + name + std::string("<value> or -") + name + std::string("=<value>");
								return ClockError::INVALID_USAGE;
							} else {
								if (!bv->setValue(argv[1])) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									return ClockError::INVALID_USAGE;
								}
								if (parsed.find(bv->getName()) != parsed.end()) {
									return ClockError::INVALID_USAGE;
								}
								parsed[bv->getName()] = true;
							}
						}
						break;
					}
				}

				if (!found) {
					error = std::string("argument -") + name + std::string(" not registered!");
					return ClockError::INVALID_USAGE;
				}
			} else {
				// TODO: (Daniel) found a single value, but no argument before
			}

			argv++;
			argc--;
		}

		return result;
	}

	std::string Parser::getLastParserError() {
		return error;
	}

} /* namespace argparser */
} /* namespace clockUtils */
