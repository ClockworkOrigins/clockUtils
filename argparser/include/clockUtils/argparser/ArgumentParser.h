#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__

#include "clockUtils/argparser/argParserParameters.h"

#include "clockUtils/argparser/BasicVariable.h"

#include <string>
#include <vector>

namespace clockUtils {
namespace argparser {

	class CLOCK_ARGPARSER_API RemoveHelper {
	public:
		RemoveHelper(const std::string & name);

		~RemoveHelper();

	private:
		std::string _name;
	};

	class CLOCK_ARGPARSER_API Parser {
	public:
		static void parseArguments(char ** argv, int argc);

		static std::vector<BasicVariable *> variableList;
	};

} /* namespace argparser */
} /* namespace clockUtils */

#define PARSE_ARGUMENTS(buffer, length)\
	clockUtils::argparser::Parser::parseArguments(buffer, length);

#define REGISTER_VARIABLE(type, name, value, description)\
	type name = value;\
	clockUtils::argparser::RemoveHelper(#name);\
	clockUtils::argparser::Parser::variableList.push_back(new clockUtils::argparser::Variable<type>(#name, description, &name));

#endif /* __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__ */
