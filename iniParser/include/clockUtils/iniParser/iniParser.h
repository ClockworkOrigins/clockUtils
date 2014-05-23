#ifndef __CLOCKUTILS_INIPARSER_INIPARSER_H__
#define __CLOCKUTILS_INIPARSER_INIPARSER_H__

#include <cstdint>
#include <sstream>
#include <string>
#include <map>
#include <tuple>
#include <vector>

#include "clockUtils/errors.h"
#include "clockUtils/iniParser/iniParserParameters.h"

namespace clockUtils {
namespace iniParser {

	class CLOCK_INIPARSER_API IniParser {
	public:
		/**
		 * \brief constructor
		 */
		IniParser();

		/**
		 * \brief loads given ini file
		 * returns ClockError::SUCCESS if no problems occured, otherwise an error code
		 */
		ClockError load(const std::string & file);

		/**
		 * \brief loads given ini file
		 * returns ClockError::SUCCESS if no problems occured, otherwise an error code
		 */
		ClockError save(const std::string & file);

		/**
		 * \brief fetches value from parsed ini file
		 * \param[in] section the section of the variable
		 * \param[in] field the name of the variable
		 * \param[out] value the variable the value should be stored in
		 * returns ClockError::SUCCESS if value found, otherwise an error code and value is set to default value of type T
		 */
		template<typename T>
		ClockError getValue(const std::string & section, const std::string & field, T & value) {
			if (_data.find(section) == _data.end()) {
				value = T();
				return ClockError::VALUE_NOTFOUND;
			}
			for (std::tuple<std::string, std::string, uint32_t, std::string> & t : _data[section]) {
				if (std::get<SECTION>(t) == section && std::get<FIELD>(t) == field) {
					std::stringstream ss(std::get<VALUE>(t));
					T result;
					if ((ss >> result).fail()) {
						value = T();
						return ClockError::WRONG_TYPE;
					}
					value = result;
					return ClockError::SUCCESS;
				}
			}

			value = T();
			return ClockError::VALUE_NOTFOUND;
		}

		/**
		 * \brief loads given ini file
		 * \param[in] section the section of the variable
		 * \param[in] field the name of the variable
		 * \param[in] value the value to be stored
		 * returns ClockError::SUCCESS if no problems occured, otherwise an error code
		 */
		template<typename T>
		void setValue(const std::string & section, const std::string & field, const T & value) {
			std::stringstream ss;
			ss << value;
			if (_data.find(section) == _data.end()) {
				// create new section
				_allLines[section].push_back("[" + section + "]");
				_data[section].push_back(make_tuple(section, field, 1, ss.str()));
				_allLines[section].push_back(""); // empty line. will be overritten anyways
				return;
			}
			for (std::tuple<std::string, std::string, uint32_t, std::string> & t : _data[section]) {
				if (std::get<SECTION>(t) == section && std::get<FIELD>(t) == field) {
					std::get<VALUE>(t) = ss.str();
					return;
				}
			}
			// value not found. Insert at end
			if (_allLines[section].back() == "" && std::get<INDEX>(_data[section].back()) != _allLines[section].size() - 1) {
				_data[section].push_back(make_tuple(section, field, _allLines[section].size() - 1, ss.str()));
			} else {
				_data[section].push_back(make_tuple(section, field, _allLines[section].size(), ss.str()));
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
		std::map<std::string, std::vector<std::tuple<std::string, std::string, uint32_t, std::string>>> _data;
		std::map<std::string, std::vector<std::string>> _allLines;
	};

} /* namespace iniParser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_INIPARSER_INIPARSER_H__ */
