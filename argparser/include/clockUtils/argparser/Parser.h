#ifndef __CLOCKUTILS_ARGPARSER_PARSER_H__
#define __CLOCKUTILS_ARGPARSER_PARSER_H__

#include "clockUtils/argparser/argParserParameters.h"

#include <vector>

namespace clockUtils {
namespace argparser {

	class BasicVariable;

	/**
	 * \brief class handling all knows variables being able to be parsed and offering parse functionality
	 */
	class CLOCK_ARGPARSER_API Parser {
		friend class BasicVariable;

	public:
		/**
		 * \brief parses argument line
		 *
		 * \param[in] argv list of all strings in the argument line
		 * \param[in] argc amount of entries in the list
		 */
		static void parseArguments(char ** argv, int argc);

	private:
		/**
		 * \brief list of all registered and parseable variables
		 */
		static std::vector<BasicVariable *> variableList;
	};

} /* namespace argparser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_PARSER_H__ */
