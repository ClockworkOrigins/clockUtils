@echo OFF

REM
REM clockUtils
REM Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
REM 
REM This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
REM modify it under the terms of the GNU Lesser General Public
REM License as published by the Free Software Foundation; either
REM version 2.1 of the License, or (at your option) any later version.
REM 
REM This library is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
REM Lesser General Public License for more details.
REM 
REM You should have received a copy of the GNU Lesser General Public
REM License along with this library; if not, write to the Free Software
REM Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
REM

SET DEP_DIR=%cd%
SET TMP_DIR=%cd%\tmp

IF "%1" == "downloadAndUnpack" (goto downloadAndUnpack)

IF [%1] == [msvc13] (
	SET VSCOMPILER=Visual Studio 12
	SET BOOSTCOMPILER=msvc-12.0
	SET ARCH_DIR=msvc13_
	SET CONFIG_BAT_PATH="%VS12%"
)
IF [%1] == [msvc15] (
	SET VSCOMPILER=Visual Studio 14
	SET BOOSTCOMPILER=msvc-14.0
	SET ARCH_DIR=msvc15_
	SET CONFIG_BAT_PATH="%VS15%"
)
IF [%1] == [android] (
	SET ARCH_DIR=android
)

IF [%2] == [32] (
	SET VSARCH=
	SET BOOSTARCH=32
	SET ARCH_DIR=%ARCH_DIR%32
	SET VSBATARCH=x86
)
IF [%2] == [64] (
	SET VSARCH= Win64
	SET BOOSTARCH=64
	SET ARCH_DIR=%ARCH_DIR%64
	SET VSBATARCH=amd64
)

IF [%CONFIG_BAT_PATH%] == [] EXIT /B

FOR %%X IN (MSBuild.exe) DO (SET FOUND=%%~$PATH:X)
IF NOT DEFINED FOUND (
	CALL %CONFIG_BAT_PATH%\vcvarsall.bat %VSBATARCH%
)

EXIT /B

:downloadAndUnpack

SET DOWNLOAD_URL=%4
IF [%4] == [] Set DOWNLOAD_URL=http://www.clockwork-origins.de/dependencies/
IF NOT EXIST %TMP_DIR% (mkdir %TMP_DIR%)
IF NOT EXIST %TMP_DIR%\%2 (bitsadmin /transfer "myDownloadJob%2" /download /priority normal %DOWNLOAD_URL%%2 %TMP_DIR%\%2)
CD %TMP_DIR%
IF EXIST %3 RD /S /Q "%2"
winrar.exe x -ibck %2
IF NOT EXIST %3 EXIT /B
EXIT /B 0

