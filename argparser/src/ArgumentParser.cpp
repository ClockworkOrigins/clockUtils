#include "clockUtils/argparser/ArgumentParser.h"

#include <iostream>

namespace clockUtils {
namespace argparser {

	std::vector<BasicVariable *> Parser::variableList = std::vector<BasicVariable *>();

	RemoveHelper::RemoveHelper(const std::string & name) : _name(name) {
	}

	RemoveHelper::~RemoveHelper() {
		for (size_t i = 0; i < Parser::variableList.size(); i++) {
			if (_name == Parser::variableList[i]->getName()) {
				delete Parser::variableList[i];
				Parser::variableList.erase(Parser::variableList.begin() + int(i));
				break;
			}
		}
	}

	void Parser::parseArguments(char ** argv, int argc) {
		while (argc > 0) {
			if (argv[0][0] == '-') {
				std::string name(&argv[0][1]);

				for (BasicVariable * bv : Parser::variableList) {
					if (name == bv->getName()) {
						if (bv->getType() == "bool") {
							bv->setValue("1");
						} else {
							if (argc == 1) {
								std::cerr << name << " requires a value: -" << name << " <value>" << std::endl;
								argv++;
								argc--;
								continue;
							}
							if (!bv->setValue(argv[1])) {
								std::cerr << argv[1] << " is not a valid value for variable " << name << std::endl;
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
