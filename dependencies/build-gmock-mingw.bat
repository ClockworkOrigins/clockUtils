@echo off

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

call build-common.bat mingw

Set ARCHIVE=gmock-1.7.0.zip
Set BUILD_DIR=%TMP_DIR%/gmock-1.7.0
Set PREFIX=%DEP_DIR%/%ARCH_DIR%/gmock

IF EXIST %PREFIX% EXIT /B

echo "Compile GoogleMock with GoogleTest"

echo "Extracting GoogleMock with GoogleTest"
if not exist %BUILD_ROOT% exit /b
cd %BUILD_ROOT%

if exist %BUILD_DIR% RD /S /Q "%BUILD_DIR%"

call build-common.bat downloadAndUnpack %ARCHIVE% %BUILD_DIR%

if not exist %BUILD_DIR% exit /b

echo "Configuring GoogleMock with GoogleTest"
cd %BUILD_DIR%
cmake . -DCMAKE_INSTALL_PREFIX=%PREFIX% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -G "%MINGWCOMPILER%" .

echo "Building GoogleMock with GoogleTest"
mingw32-make

echo "Installing GoogleMock with GoogleTest"
mkdir "%PREFIX%"
mkdir "%PREFIX%/include"
mkdir "%PREFIX%/lib"
xcopy /S /Y "%BUILD_DIR%/gtest/include" "%PREFIX%/include" > NUL
xcopy /S /Y "%BUILD_DIR%/include" "%PREFIX%/include" > NUL

xcopy /F "%BUILD_DIR%/libgmock.dll" "%PREFIX%/lib/libgmock.dll*" > NUL
xcopy /F "%BUILD_DIR%/libgmock.dll.a" "%PREFIX%/lib/libgmock.dll.a*" > NUL
xcopy /F "%BUILD_DIR%/libgmock_main.dll" "%PREFIX%/lib/libgmock_main.dll*" > NUL
xcopy /F "%BUILD_DIR%/libgmock_main.dll.a" "%PREFIX%/lib/libgmock_main.dll.a*" > NUL

xcopy /F "%BUILD_DIR%/gtest/libgtest.dll" "%PREFIX%/lib/libgtest.dll*" > NUL
xcopy /F "%BUILD_DIR%/gtest/libgtest.dll.a" "%PREFIX%/lib/libgtest.dll.a*" > NUL
xcopy /F "%BUILD_DIR%/gtest/libgtest_main.dll" "%PREFIX%/lib/libgtest_main.dll*" > NUL
xcopy /F "%BUILD_DIR%/gtest/libgtest_main.dll.a" "%PREFIX%/lib/libgtest_main.dll.a*" > NUL

echo "Cleaning up"
cd %DEP_DIR%
RD /S /Q "%BUILD_DIR%"
RD /S /Q "%TMP_DIR%"
