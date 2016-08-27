#!/bin/bash

# clockUtils
# Copyright (2016) Daniel Bonrath, Michael Baer, All rights reserved.
#
# This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

cd "$(readlink -f "$(dirname "${0}")")"

FILE=${1}
URL=http://www.clockwork-origins.de/dependencies/
if [ -n "$2" ]; then
	URL=${2}
fi

if ! [ -f "${BUILD_ROOT}/${FILE}" ]; then
	wget ${URL}/${FILE} -P ${BUILD_ROOT}
fi

