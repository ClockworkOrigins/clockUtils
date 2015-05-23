#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__

#include "clockUtils/argParser/argParserParameters.h"

#include "clockUtils/argParser/BasicVariable.h"
#include "clockUtils/argParser/Parser.h"

/**
 * \brief parses given list with given length
 */
#define PARSE_ARGUMENTS(buffer, length)\
	clockUtils::argParser::Parser::parseArguments(buffer, length)

/**
 * \brief parses command line
 */
#define PARSE_COMMANDLINE()\
	clockUtils::argParser::Parser::parseArguments(argv, argc)

/**
 * \brief returns the last error message if some appeared
 */
#define GETLASTPARSERERROR()\
	clockUtils::argParser::Parser::getLastParserError()

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help
 */
#define REGISTER_VARIABLE(type, name, value, description)\
	clockUtils::argParser::Variable<type> name(#name, description, value)

/**
 * \brief registers a variable where the arguments at the end are parsed into
 */
#define REGISTER_VARIABLE_ARGUMENTS(name)\
	std::vector<std::string> name;\
	clockUtils::argParser::Parser::registerArgumentsVector(&name)

#endif /* __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__ */
