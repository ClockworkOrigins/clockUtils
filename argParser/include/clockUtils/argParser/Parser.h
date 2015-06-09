#ifndef __CLOCKUTILS_ARGPARSER_PARSER_H__
#define __CLOCKUTILS_ARGPARSER_PARSER_H__

#include "clockUtils/argParser/argParserParameters.h"

#include <map>
#include <string>
#include <vector>

namespace clockUtils {
	enum class ClockError;
namespace argParser {

	class BasicVariable;

	/**
	 * \brief class handling all known variables being able to be parsed and offering parse functionality
	 */
	class CLOCK_ARGPARSER_API Parser {
		friend class BasicVariable;

	public:
		/**
		 * \brief parses argument line
		 * \param[in] argv list of all strings in the argument line
		 * \param[in] argc amount of entries in the list
		 */
		static ClockError parseArguments(const char ** argv, int argc);

		/**
		 * \brief returns last error or empty string
		 */
		static std::string getLastParserError();

		/**
		 * \brief registers a vector for arguments at end of input
		 */
		static void registerArgumentsVector(std::vector<std::string> * args) {
			arguments = args;
		}

		/**
		 * \brief sets bool determining whether an error shall occure if an extra flag is given that wasn't registered
		 */
		static void setErrorOnFlag(bool b) {
			errorOnFlag = b;
		}

		/**
		 * \brief returns true if --help was found in argument line
		 */
		static bool wasHelpSet() {
			return help;
		}

		/**
		 * \brief returns help text as string
		 */
		static std::string getHelpText();

		/**
		 * \brief add a line for help text
		 */
		static void addHelpTextLine(const std::pair<std::string, std::string> & line) {
			helpTexts.insert(line);
		}

	private:
		/**
		 * \brief list of all registered and parseable variables
		 */
		static std::vector<BasicVariable *> variableList;

		/**
		 * \brief contains the last error being detected or an empty string
		 */
		static std::string error;

		/**
		 * \brief vector containing all parsed arguments at the end of the input
		 */
		static std::vector<std::string> * arguments;

		/**
		 * \brief bool determining whether an error shall occure if an extra flag is given that wasn't registered
		 */
		static bool errorOnFlag;

		/**
		 * \brief bool true if --help was found in argument line
		 */
		static bool help;

		/**
		 * \brief map containing all lines for help text
		 */
		static std::map<std::string, std::string> helpTexts;
	};

} /* namespace argParser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_PARSER_H__ */
