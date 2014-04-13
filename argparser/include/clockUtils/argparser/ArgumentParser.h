#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__

#include "clockUtils/argparser/argParserParameters.h"

#include "clockUtils/argparser/BasicVariable.h"
#include "clockUtils/argparser/Parser.h"

/**
 * \brief parses given list with given length
 */
#define PARSE_ARGUMENTS(buffer, length)\
	clockUtils::argparser::Parser::parseArguments(buffer, length)

/**
 * \brief parses command line
 */
#define PARSE_COMMANDLINE()\
	clockUtils::argparser::Parser::parseArguments(argv, argc)

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help
 */
#define REGISTER_VARIABLE(type, name, value, description)\
	clockUtils::argparser::Variable<type> name(#name, description, value)

#endif /* __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__ */
