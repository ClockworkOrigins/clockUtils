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
 * \addtogroup iniParser
 * @{
 */

#ifndef __CLOCKUTILS_INIPARSER_INIPARSER_H__
#define __CLOCKUTILS_INIPARSER_INIPARSER_H__

#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "clockUtils/errors.h"
#include "clockUtils/iniParser/iniParserParameters.h"

namespace clockUtils {
namespace iniParser {

	/**
	 * \brief this class is used for parsing configuration files
	 */
	class CLOCK_INIPARSER_API IniParser {
	public:
		/**
		 * \brief constructor
		 */
		IniParser();

		/**
		 * \brief loads given ini file
		 * \returns ClockError::SUCCESS if no problems occured, otherwise an error code
		 */
		ClockError load(const std::string & file);

		/**
		 * \brief saves to given ini file
		 * \returns ClockError::SUCCESS if no problems occured, otherwise an error code
		 */
		ClockError save(const std::string & file);

		/**
		 * \brief fetches value from parsed ini file
		 * \param[in] section the section of the variable
		 * \param[in] field the name of the variable
		 * \param[out] value the variable the value should be stored in
		 * \returns ClockError::SUCCESS if value found, otherwise an error code.
		 * If the section or field was not found, value stays unchanged. Otherwise the streamoperator will be called
		 */
		template<typename T>
		typename std::enable_if<!std::is_enum<T>::value, ClockError>::type getValue(const std::string & section, const std::string & field, T & value) const {
			auto it = _data.find(section);
			if (it == _data.end()) {
				return ClockError::VALUE_NOTFOUND;
			}
			for (const std::tuple<std::string, std::string, size_t, std::string> & t : it->second) {
				if (std::get<SECTION>(t) == section && std::get<FIELD>(t) == field) {
					std::stringstream ss(std::get<VALUE>(t));
					if ((ss >> value).fail() || !ss.eof()) {
						return ClockError::WRONG_TYPE;
					}
					return ClockError::SUCCESS;
				}
			}

			return ClockError::VALUE_NOTFOUND;
		}

		template<typename T>
		typename std::enable_if<std::is_enum<T>::value, ClockError>::type getValue(const std::string & section, const std::string & field, T & value) const {
			auto it = _data.find(section);
			if (it == _data.end()) {
				return ClockError::VALUE_NOTFOUND;
			}
			for (const std::tuple<std::string, std::string, size_t, std::string> & t : it->second) {
				if (std::get<SECTION>(t) == section && std::get<FIELD>(t) == field) {
					std::stringstream ss(std::get<VALUE>(t));
					int v;
					if ((ss >> v).fail() || !ss.eof()) {
						return ClockError::WRONG_TYPE;
					}
					value = T(v);
					return ClockError::SUCCESS;
				}
			}

			return ClockError::VALUE_NOTFOUND;
		}

		/**
		 * \brief sets value for a variable
		 * \param[in] section the section of the variable
		 * \param[in] field the name of the variable
		 * \param[in] value the value to be stored
		 */
		template<typename T>
		typename std::enable_if<!std::is_enum<T>::value, void>::type setValue(const std::string & section, const std::string & field, const T & value) {
			std::stringstream ss;
			ss << value;
			setValue(section, field, ss.str());
		}

		template<typename T>
		typename std::enable_if<std::is_enum<T>::value, void>::type setValue(const std::string & section, const std::string & field, const T & value) {
			std::stringstream ss;
			ss << int(value);
			setValue(section, field, ss.str());
		}

		void setValue(const std::string & section, const std::string & field, const std::string & value) {
			if (_data.find(section) == _data.end()) {
				// create new section
				_allLines[section].push_back("[" + section + "]");
				_data[section].push_back(make_tuple(section, field, 1, value));
				_allLines[section].push_back(""); // empty line. will be overritten anyways
				return;
			}
			for (std::tuple<std::string, std::string, size_t, std::string> & t : _data[section]) {
				if (std::get<SECTION>(t) == section && std::get<FIELD>(t) == field) {
					std::get<VALUE>(t) = value;
					return;
				}
			}
			// value not found. Insert at end
			if (_allLines[section].back() == "" && std::get<INDEX>(_data[section].back()) != _allLines[section].size() - 1) {
				_data[section].push_back(make_tuple(section, field, _allLines[section].size() - 1, value));
			} else {
				_data[section].push_back(make_tuple(section, field, _allLines[section].size(), value));
			}
			_allLines[section].push_back(""); // empty line. will be overritten anyways
		}

	private:
		/**
		 * \brief enum to identify entries in tuple
		 */
		enum Fields {
			SECTION,
			FIELD,
			INDEX,
			VALUE
		};

		// one vector with lines/values for each section
		std::map<std::string, std::vector<std::tuple<std::string, std::string, size_t, std::string>>> _data;
		std::map<std::string, std::vector<std::string>> _allLines;
	};

	/**
	 * \brief override for std::string so a full line containing spaces will be returned
	 */
	template<>
	ClockError CLOCK_INIPARSER_API IniParser::getValue<std::string>(const std::string & section, const std::string & field, std::string & value) const;

} /* namespace iniParser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_INIPARSER_INIPARSER_H__ */

/**
 * @}
 */
