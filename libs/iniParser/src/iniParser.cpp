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

#include "clockUtils/iniParser/iniParser.h"

#include <algorithm>
#include <fstream>

#include "clockUtils/util/string.h"

namespace clockUtils {
namespace iniParser {

	IniParser::IniParser() : _data(), _allLines() {
	}

	ClockError IniParser::load(const std::string & file) {
		std::fstream fs;
		fs.open(file, std::fstream::in);

		if (fs.fail()) {
			return ClockError::FILENOTFOUND;
		}

		std::string currentSection = "global";
		// reset values
		_allLines.clear();
		_data.clear();
		_allLines["global"];
		_data["global"];

		std::string line = "";
		while (fs.good()) {
			getline(fs, line);

			// only trim from left to avoid killing value spaces
			ltrim(line);

			if (line.empty() || line.length() == 1 || line.at(0) == ';') {
				_allLines[currentSection].push_back(line);
				continue; // no entry found
			}

			if (line.at(0) == '[') { // section or error
				// now trim right side as well
				rtrim(line);

				if (line.at(line.length() - 1) != ']') {
					// "Couldn't parse ini file! Section not closed!"
					return ClockError::WRONG_SYNTAX;
				}
				currentSection = line.substr(1, line.length() - 2);
				if (_allLines.find(currentSection) != _allLines.end()) {
					// section already exists
					return ClockError::WRONG_SYNTAX;
				}
			} else { // found field
				size_t n = line.find("=");

				if (n == std::string::npos) {
					// "Couldn't parse ini file! Found field without ="
					return ClockError::WRONG_SYNTAX;
				}

				std::string key = line.substr(0, n);
				trim(key);

				for (auto & t : _data[currentSection]) {
					if (std::get<FIELD>(t) == key) {
						// already exists
						return ClockError::WRONG_SYNTAX;
					}
				}

				// do not trim value. everything is significant
				std::string value = line.substr(n + 1, line.length() - n - 1);
				_data[currentSection].push_back(std::make_tuple(currentSection, key, _allLines[currentSection].size(), value));
			}
			_allLines[currentSection].push_back(line);
		}

		fs.close();
		return ClockError::SUCCESS;
	}

	ClockError IniParser::save(const std::string & file) {
		std::fstream fs;
		fs.open(file, std::fstream::out);

		if (fs.bad()) {
			return ClockError::FILENOTFOUND;
		}

		std::stringstream ss;

		for (std::pair<std::string, std::vector<std::string>> p : _allLines) {
			uint32_t nextEntry = 0;
			for (size_t i = 0; i < p.second.size(); i++) {
				if (nextEntry < _data[p.first].size() && i == std::get<INDEX>(_data[p.first][nextEntry])) {
					ss << std::get<FIELD>(_data[p.first][nextEntry]) << "=" << std::get<VALUE>(_data[p.first][nextEntry]) << "\n";
					nextEntry++;
				} else {
					ss << p.second[i] << '\n';
				}
			}
		}

		fs << ss.str().substr(0, ss.str().length() - 1);

		fs.flush();
		fs.close();

		return ClockError::SUCCESS;
	}

	template<>
	ClockError IniParser::getValue<std::string>(const std::string & section, const std::string & field, std::string & value) const {
		auto it = _data.find(section);
		if (it == _data.end()) {
			value = std::string();
			return ClockError::VALUE_NOTFOUND;
		}
		for (const std::tuple<std::string, std::string, size_t, std::string> & t : it->second) {
			if (std::get<SECTION>(t) == section && std::get<FIELD>(t) == field) {
				value = std::get<VALUE>(t);
				return ClockError::SUCCESS;
			}
		}

		value = std::string();
		return ClockError::VALUE_NOTFOUND;
	}

	std::vector<std::string> IniParser::getAllSections() const {
		std::vector<std::string> sections;
		for (auto it = _allLines.cbegin(); it != _allLines.cend(); it++) {
			sections.push_back(it->first);
		}
		return sections;
	}

	std::vector<std::string> IniParser::getAllEntries(const std::string & section) const {
		std::vector<std::string> entries;
		auto it = _data.find(section);
		if (it != _data.end()) {
			for (auto it2 = it->second.cbegin(); it2 != it->second.cend(); it2++) {
				entries.push_back(std::get<FIELD>(*it2));
			}
		}
		return entries;
	}

	void IniParser::removeEntry(const std::string & section, const std::string & entry) {
		for (auto it = _data[section].begin(); it != _data[section].end(); it++) {
			if (std::get<SECTION>(*it) == section && std::get<FIELD>(*it) == entry) {
				_allLines[section].erase(_allLines[section].begin() + std::get<INDEX>(*it));
				_data[section].erase(it);
				break;
			}
		}
		if (_data[section].empty()) {
			_data.erase(section);
			_allLines.erase(section);
		}
	}

} /* namespace iniParser */
} /* namespace clockUtils */
