/*
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

/**
 * \addtogroup argParser
 * @{
 */

#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTPARSER_H__

#include "clockUtils/argParser/argParserParameters.h"

#include <algorithm>

#include "clockUtils/argParser/ArgumentList.h"
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
 * longname is also the variable name, shortname can be set to "" for no shortname
 * variable will be initialized with default value right away
 * \note help and h are reserved for internal usage only
 */
#define REGISTER_VARIABLE_INTERNAL(type, longname, shortname, value, description, required, multiple)\
	{\
		std::string helpText = std::string("\t--") + std::string(#longname);\
		if (std::string(#shortname) != "\"\"") {\
			helpText += std::string(", -") + std::string(#shortname);\
		}\
		size_t length = helpText.length();\
		for (; length < 16; length++) {\
			helpText += " ";\
		}\
		helpText += std::string("[Default: ") + std::string(#value) + std::string("]");\
		helpText += std::string(std::max(0, int(15 - std::string(#value).length())), ' ');\
		helpText += std::string(description);\
		if (required) {\
			helpText += " (required)";\
		}\
		if (multiple) {\
			helpText += " (multiple)";\
		}\
		clockUtils::argParser::Parser::addHelpTextLine(std::make_pair(#longname, helpText));\
	}\
	clockUtils::argParser::Variable<type> longname(#longname, #shortname, description, value, required, multiple)

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help being optional
 * longname is also the variable name, shortname can be set to "" for no shortname
 * variable will be initialized with default value right away
 * \note help and h are reserved for internal usage only
 */
#define REGISTER_VARIABLE(type, longname, shortname, value, description) REGISTER_VARIABLE_INTERNAL(type, longname, shortname, value, description, false, false)

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help being required
 * longname is also the variable name, shortname can be set to "" for no shortname
 * variable will be initialized with default value right away
 * if this variable isn't set, ClockError::INVALID_USAGE is returned
 * \note help and h are reserved for internal usage only
 */
#define REGISTER_VARIABLE_REQUIRED(type, longname, shortname, value, description) REGISTER_VARIABLE_INTERNAL(type, longname, shortname, value, description, true, false)

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help
 * longname is also the variable name, shortname can be set to "" for no shortname
 * variable will be initialized with default value right away
 * this variable can be set multiple times, so the registered variable is a list
 * \note help and h are reserved for internal usage only
 */
#define REGISTER_VARIABLE_MULTIPLE(type, longname, shortname, value, description) REGISTER_VARIABLE_INTERNAL(type, longname, shortname, value, description, false, true)

/**
 * \brief registers a variable with a type, the variable and argument name, a default value and a description text for --help being required
 * longname is also the variable name, shortname can be set to "" for no shortname
 * variable will be initialized with default value right away
 * this variable can be set multiple times, so the registered variable is a list
 * if this variable isn't set, ClockError::INVALID_USAGE is returned
 * \note help and h are reserved for internal usage only
 */
#define REGISTER_VARIABLE_MULTIPLE_REQUIRED(type, longname, shortname, value, description) REGISTER_VARIABLE_INTERNAL(type, longname, shortname, value, description, true, true)

/**
 * \brief registers a variable where the arguments at the end are parsed into
 */
#define REGISTER_VARIABLE_ARGUMENTS(name)\
	clockUtils::argParser::ArgumentList name;

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

/**
 * @}
 */
