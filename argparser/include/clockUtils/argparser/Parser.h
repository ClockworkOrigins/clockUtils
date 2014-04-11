#ifndef __CLOCKUTILS_ARGPARSER_PARSER_H__
#define __CLOCKUTILS_ARGPARSER_PARSER_H__

#include "clockUtils/argparser/argParserParameters.h"

#include <vector>

namespace clockUtils {
namespace argparser {

	class BasicVariable;

	class CLOCK_ARGPARSER_API Parser {
	public:
		static void parseArguments(char ** argv, int argc);

		static std::vector<BasicVariable *> variableList;
	};

} /* namespace argparser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_PARSER_H__ */
