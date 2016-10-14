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

#ifndef __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__
#define __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__

#include "clockUtils/argParser/argParserParameters.h"

#include <sstream>
#include <string>
#include <vector>

namespace clockUtils {
namespace argParser {

	class Parser;

	/**
	 * \brief base class for Variable handling
	 */
	class CLOCK_ARGPARSER_API BasicVariable {
		friend class Parser;

	public:
		/**
		 * \brief constructor taking the longname (= argument in argument list), shortname (= short name for the argument list) and a description text for --help, inserts variable into the variableList of the Parser
		 */
		BasicVariable(const std::string & longname, const std::string & shortname, const std::string & description, bool required, bool multiple);

		/**
		 * \brief destructor, removes variable of the variableList of the Parser
		 */
		virtual ~BasicVariable();

		/**
		 * \brief returns the argument of the variable
		 */
		inline std::string getLongname() const {
			return _longname;
		}

		/**
		 * \brief returns the argument of the variable
		 */
		inline std::string getShortname() const {
			return _shortname;
		}

		/**
		 * \brief returns whether the type of the variable is bool (true) or not (false)
		 */
		virtual bool isBool() const = 0;

		/**
		 * \brief sets a value received from parsing
		 */
		virtual bool setValue(const std::string & value) = 0;

		/**
		 * \brief returns true, if this variable was set via command line
		 */
		bool isSet() const {
			return _set;
		}

		/**
		 * \brief returns true, if this variable is required to be set
		 */
		bool isRequired() const {
			return _required;
		}

		/**
		 * \brief returns true, if this variable can have multiple values
		 */
		bool canHaveMultiple() const {
			return _multiple;
		}

		/**
		 * \brief resets value to default value
		 */
		virtual void resetToDefault() = 0;

	protected:
		/**
		 * \brief tells whether the variable was set via command line or not
		 */
		bool _set;

	private:
		/**
		 * \brief argument the argument list is scanned for
		 */
		std::string _longname;

		/**
		 * \brief short argument the argument list is scanned for
		 */
		std::string _shortname;

		/**
		 * \brief description being shown using --help
		 */
		std::string _description;
		
		/**
		 * \brief tells whether the variable has to be set or not
		 */
		bool _required;
		
		/**
		 * \brief tells whether the variable can have multiple values or not
		 */
		bool _multiple;
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
		Variable(const std::string & longname, const std::string & shortname, const std::string & description, T value, bool required, bool multiple) : BasicVariable(longname, shortname, description, required, multiple), _value(value), _values(), _defaultValue(value) {
		}

		/**
		 * \brief returns true, if T is bool, otherwise false. Default implementation always returns false, for bool this method is specialized
		 */
		bool isBool() const override {
			return std::is_same<T, bool>::value;
		}

		/**
		 * \brief sets the parsed value of the argument and returns true, if value was valid, otherwise false
		 */
		bool setValue(const std::string & value) override {
			std::stringstream ss(value);
			T val;
			bool ret = !(ss >> (val)).fail() && ss.eof();
			if (!_set) {
				_value = val;
			} else {
				_values.push_back(val);
			}
			return ret;
		}

		/**
		 * \brief operator comparing type T and Variable containing type T
		 */
		friend bool operator==(const T & first, const Variable<T> & second) {
			return first == second._value;
		}

		friend bool operator==(const Variable<T> & first, const T & second) {
			return first._value == second;
		}

		friend bool operator!=(const T & first, const Variable<T> & second) {
			return first != second._value;
		}

		friend bool operator!=(const Variable<T> & first, const T & second) {
			return first._value != second;
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

		/**
		 * \brief stream operator
		 */
		friend std::ostream & operator<<(std::ostream & out, Variable & v) {
			out << v._value;
			return out;
		}

		/**
		 * \brief returns amount of values provided if using multiple
		 */
		size_t count() const {
			return 1 + _values.size();
		}

		/**
		 * \brief returns value at position idx, idx = 0 is _value, idx = 1 is first element of _values, idx >= count results in undefined behaviour
		 */
		T at(size_t idx) const {
			return (idx == 0) ? _value : _values[idx - 1];
		}

	private:
		/**
		 * \brief the value this variable contains
		 */
		T _value;

		/**
		 * \brief the values this variable contains when using multiples
		 */
		std::vector<T> _values;

		/**
		 * \brief the default value for this variable
		 * stored in case it is parsed twice to be able to reset value to default
		 */
		T _defaultValue;

		/**
		 * \brief resets value to default value
		 */
		void resetToDefault() override {
			_value = _defaultValue;
			_values.clear();
			_set = false;
		}
	};

	/**
	 * \brief specialization of setValue for std::string so it can work with strings containing spaces
	 */
	template<>
	bool CLOCK_ARGPARSER_API Variable<std::string>::setValue(const std::string & value);

	/**
	 * \brief specialization of setValue for char because chars contain only one single character
	 */
	template<>
	bool CLOCK_ARGPARSER_API Variable<char>::setValue(const std::string & value);

	/**
	 * \brief specialization of setValue for bool because it shall also allow usage of true and false
	 */
	template<>
	bool CLOCK_ARGPARSER_API Variable<bool>::setValue(const std::string & value);

	template
	class CLOCK_ARGPARSER_API Variable<bool>;

	template
	class CLOCK_ARGPARSER_API Variable<char>;

	template
	class CLOCK_ARGPARSER_API Variable<std::string>;

} /* namespace argParser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__ */

/**
 * @}
 */
