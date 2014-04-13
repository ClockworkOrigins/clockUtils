#include "clockUtils/argparser/Parser.h"

#include <iostream>

#include "clockUtils/errors.h"
#include "clockUtils/argparser/BasicVariable.h"

namespace clockUtils {
namespace argparser {

	std::vector<BasicVariable *> Parser::variableList = std::vector<BasicVariable *>();

	ClockError Parser::parseArguments(char ** argv, int argc) {
		while (argc > 0) {
			if (argv[0][0] == '-' && argv[0][1] != '\0') {
				std::string name(&argv[0][1]);

				bool found = false;

				for (BasicVariable * bv : Parser::variableList) {
					if (name.find(bv->getName()) == 0) {
						found = true;
						if (bv->isBool()) {
							bv->setValue("1");
						} else {
							if (name.length() > bv->getName().length()) {
								size_t startIndex = bv->getName().length();
								if (name.at(startIndex) == '=') {
									startIndex++;
								}
								if (!bv->setValue(name.substr(startIndex, name.length()))) {
									std::cerr << argv[1] << " is not a valid value for variable " << name << std::endl;
									return ClockError::INVALID_USAGE;
								}
							} else if (argc == 1) {
								std::cerr << name << " requires a value: -" << name << " <value> or -" << name << "<value> or -" << name << "=<value>" << std::endl;
								return ClockError::INVALID_USAGE;
							} else {
								if (!bv->setValue(argv[1])) {
									std::cerr << argv[1] << " is not a valid value for variable " << name << std::endl;
									return ClockError::INVALID_USAGE;
								}
							}
						}
					}
				}

				if (!found) {
					std::cerr << "argument -" << name << " not registered!" << std::endl;
					return ClockError::INVALID_ARGUMENT;
				}
			} else {
				// TODO: (Daniel) found a single value, but no argument before
			}

			argv++;
			argc--;
		}

		return ClockError::SUCCESS;
	}

} /* namespace argparser */
} /* namespace clockUtils */
