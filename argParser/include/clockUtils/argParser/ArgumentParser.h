#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__

#include "clockUtils/argParser/argParserParameters.h"

#include "clockUtils/argParser/BasicVariable.h"
#include "clockUtils/argParser/Parser.h"

/**
 * \brief parses given list with given length
 * use this method without filename
 */
#define PARSE_ARGUMENTS(buffer, length)\
	clockUtils::argParser::Parser::parseArguments(buffer, length)

/**
 * \brief parses command line
 * Use this to parse the normal command line in the main function.
 * It is identical to PARSE_ARGUMENTS(++argv, --argc)
 */
#define PARSE_COMMANDLINE()\
	clockUtils::argParser::Parser::parseArguments(++argv, --argc)

/**
 * \brief returns the last error message if some appeared
 */
#define GETLASTPARSERERROR()\
	clockUtils::argParser::Parser::getLastParserError()

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help
 */
#define REGISTER_VARIABLE(type, name, value, description)\
	{\
		clockUtils::argParser::Parser::addHelpTextLine(std::make_pair(#name, std::string("\t-") + std::string(#name) + std::string("\t[Default: ") + std::string(#value) + std::string("]\t\t") + std::string(description)));\
	}\
	clockUtils::argParser::Variable<type> name(#name, description, value)

/**
 * \brief registers a variable where the arguments at the end are parsed into
 */
#define REGISTER_VARIABLE_ARGUMENTS(name)\
	std::vector<std::string> name;\
	clockUtils::argParser::Parser::registerArgumentsVector(&name)

/**
 * \brief returns true if --help was set
 */
#define HELPSET()\
	clockUtils::argParser::Parser::wasHelpSet()

/**
 * \brief returns the help text
 */
#define GETHELPTEXT()\
	clockUtils::argParser::Parser::getHelpText()

#endif /* __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__ */
