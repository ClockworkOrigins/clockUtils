#ifndef __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__
#define __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__

#include "clockUtils/argParser/argParserParameters.h"

#include <sstream>
#include <string>

namespace clockUtils {
namespace argParser {

	/**
	 * \brief base class for Variable handling
	 */
	class CLOCK_ARGPARSER_API BasicVariable {
	public:
		/**
		 * \brief constructor taking the name (= argument in argument list) and a description text for --help, inserts variable into the variableList of the Parser
		 */
		BasicVariable(const std::string & name, const std::string & description);

		/**
		 * \brief destructor, removes variable of the variableList of the Parser
		 */
		virtual ~BasicVariable();

		/**
		 * \brief returns the argument of the variable
		 */
		inline std::string getName() const {
			return _name;
		}

		/**
		 * \brief returns whether the type of the variable is bool (true) or not (false)
		 */
		virtual bool isBool() const = 0;

		/**
		 * \brief sets a value received from parsing
		 */
		virtual bool setValue(const std::string & value) = 0;

	private:
		/**
		 * \brief argument the argument list is scanned for
		 */
		std::string _name;

		/**
		 * \brief description being shown using --help
		 */
		std::string _description;
	};

	/**
	 * \brief specialization of the variable, depending on the template type
	 */
	template<typename T>
	class Variable : public BasicVariable {
	public:
		/**
		 * \brief initializes a new variable taking the argument name, the description text and a default value, calls constructor of BasicVariable
		 */
		Variable(const std::string & name, const std::string & description, T value) : BasicVariable(name, description), _value(value) {
		}

		/**
		 * \brief returns true, if T is bool, otherwise false. Default implementation always returns false, for bool this method is specialized
		 */
		bool isBool() const {
			return false;
		}

		/**
		 * \brief sets the parsed value of the argument and returns true, if value was valid, otherwise false
		 */
		bool setValue(const std::string & value) {
			std::stringstream ss(value);
			return !(ss >> (_value)).fail();
		}

		/**
		 * \brief operator comparing type T and Variable containing type T
		 */
		friend bool operator==(const T & first, const Variable<T> & second) {
			return first == second._value;
		}

		/**
		 * \brief assignment operator taking type T
		 */
		T & operator=(const T & val) {
			_value = val;
			return _value;
		}

		/**
		 * \brief operator casting Variable<T> to T
		 */
		operator T() const {
			return _value;
		}

	private:
		/**
		 * \brief the value this variable contains
		 */
		T _value;
	};

	/**
	* \brief specialization of isBool method for type bool, returns always true
	*/
	template<>
	bool CLOCK_ARGPARSER_API Variable<bool>::isBool() const;

} /* namespace argParser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__ */
