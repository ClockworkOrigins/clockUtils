# clockUtils
# Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
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

IF(WITH_LIBRARY_ARGPARSER)
	ADD_SUBDIRECTORY(argParser)
ENDIF(WITH_LIBRARY_ARGPARSER)

IF(WITH_LIBRARY_COMPRESSION)
	ADD_SUBDIRECTORY(compression)
ENDIF(WITH_LIBRARY_COMPRESSION)

IF(WITH_LIBRARY_CONTAINER)
	ADD_SUBDIRECTORY(container)
ENDIF(WITH_LIBRARY_CONTAINER)

IF(WITH_LIBRARY_INIPARSER)
	ADD_SUBDIRECTORY(iniParser)
ENDIF(WITH_LIBRARY_INIPARSER)

IF(WITH_LIBRARY_LOG)
	ADD_SUBDIRECTORY(log)
ENDIF(WITH_LIBRARY_LOG)

IF(WITH_LIBRARY_SOCKETS)
	ADD_SUBDIRECTORY(sockets)
ENDIF(WITH_LIBRARY_SOCKETS)
