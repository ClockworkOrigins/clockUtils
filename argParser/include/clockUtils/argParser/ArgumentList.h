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

#ifndef __CLOCKUTILS_ARGPARSER_ARGUMENTLIST_H__
#define __CLOCKUTILS_ARGPARSER_ARGUMENTLIST_H__

#include "clockUtils/argParser/argParserParameters.h"

#include <string>
#include <vector>

namespace clockUtils {
namespace argParser {

	class Parser;

	/**
	 * \brief base class for Variable handling
	 */
	class CLOCK_ARGPARSER_API ArgumentList {
		friend class Parser;

	public:
		typedef std::vector<std::string>::iterator iterator;
		typedef std::vector<std::string>::const_iterator const_iterator;
		typedef std::vector<std::string>::reverse_iterator reverse_iterator;
		typedef std::vector<std::string>::const_reverse_iterator const_reverse_iterator;
		typedef std::vector<std::string>::reference reference;
		typedef std::vector<std::string>::const_reference const_reference;
		typedef std::vector<std::string>::size_type size_type;

		/**
		 * \brief constructor creating empty std::vector<std::string> and registering it in parser
		 */
		ArgumentList();

		/**
		 * \brief destructor, removes list of the Parser
		 */
		~ArgumentList();

		/**
		 * \brief all following methods are just wrappers for std::vector
		 */
		iterator begin() {
			return _vec.begin();
		}

		const_iterator begin() const {
			return _vec.begin();
		}

		iterator end() {
			return _vec.end();
		}

		const_iterator end() const {
			return _vec.end();
		}

		reverse_iterator rbegin() {
			return _vec.rbegin();
		}

		const_reverse_iterator rbegin() const {
			return _vec.rbegin();
		}

		reverse_iterator rend() {
			return _vec.rend();
		}

		const_reverse_iterator rend() const {
			return _vec.rend();
		}

		const_iterator cbegin() const {
			return _vec.cbegin();
		}

		const_iterator cend() const {
			return _vec.cend();
		}

		const_reverse_iterator crbegin() const {
			return _vec.crbegin();
		}

		const_reverse_iterator crend() const {
			return _vec.crend();
		}

		size_t size() const {
			return _vec.size();
		}

		bool empty() const {
			return _vec.empty();
		}

		reference operator[](size_type n) {
			return _vec[n];
		}

		const_reference operator[](size_type n) const {
			return _vec[n];
		}

		reference at(size_type n) {
			return _vec.at(n);
		}

		const_reference at(size_type n) const {
			return _vec.at(n);
		}

		reference front() {
			return _vec.front();
		}

		const_reference front() const {
			return _vec.front();
		}

		reference back() {
			return _vec.back();
		}

		const_reference back() const {
			return _vec.back();
		}

	private:
		/**
		 * \brief the wrapped vector
		 */
		std::vector<std::string> _vec;
	};

} /* namespace argParser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_ARGUMENTLIST_H__ */

/**
 * @}
 */
