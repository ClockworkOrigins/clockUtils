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
#include <iostream>
namespace clockUtils {
namespace iniParser {

	class CLOCK_INIPARSER_API IniParser {
	public:
		IniParser();

		ClockError load(const std::string & file);
		ClockError save(const std::string & file);

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
