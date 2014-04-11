#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__

#include "clockUtils/argparser/argParserParameters.h"

#include "clockUtils/argparser/BasicVariable.h"
#include "clockUtils/argparser/Parser.h"

#include <string>
#include <vector>

namespace clockUtils {
namespace argparser {

} /* namespace argparser */
} /* namespace clockUtils */

#define PARSE_ARGUMENTS(buffer, length)\
	clockUtils::argparser::Parser::parseArguments(buffer, length);

#define REGISTER_VARIABLE(type, name, value, description)\
	clockUtils::argparser::Variable<type> name(#name, description, value);

#endif /* __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__ */
