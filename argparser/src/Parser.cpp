#include "clockUtils/argparser/Parser.h"

#include "clockUtils/argparser/BasicVariable.h"

#include <iostream>

namespace clockUtils {
namespace argparser {

	std::vector<BasicVariable *> Parser::variableList = std::vector<BasicVariable *>();

	void Parser::parseArguments(char ** argv, int argc) {
		while (argc > 0) {
			if (argv[0][0] == '-') {
				std::string name(&argv[0][1]);

				for (BasicVariable * bv : Parser::variableList) {
					if (name.find(bv->getName()) == 0) {
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
								}
							} else if (argc == 1) {
								std::cerr << name << " requires a value: -" << name << " <value> or -" << name << "<value> or -" << name << "=<value>" << std::endl;
								argv++;
								argc--;
								continue;
							} else {
								if (!bv->setValue(argv[1])) {
									std::cerr << argv[1] << " is not a valid value for variable " << name << std::endl;
								}
							}
						}
					}
				}
			}

			argv++;
			argc--;
		}
	}

} /* namespace argparser */
} /* namespace clockUtils */
