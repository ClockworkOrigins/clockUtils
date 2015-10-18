@echo off

SET TOOLCHAIN=Visual Studio 12
SET ARCH=
SET PREFIXFOLDER_COMPILER=msvc12
SET PREFIXFOLDER_ARCH=x86
IF [%1] == [msvc12] (
	SET TOOLCHAIN=Visual Studio 12
	SET PREFIXFOLDER_COMPILER=msvc12
)
IF [%1] == [msvc14] (
	SET TOOLCHAIN=Visual Studio 14
	SET PREFIXFOLDER_COMPILER=msvc14
)
IF [%2] == [64] (
	SET ARCH= Win64
	SET PREFIXFOLDER_ARCH=x64
)
IF [%2] == [32] (
	SET ARCH=
	SET PREFIXFOLDER_ARCH=x86
)
IF [%2] == [arm] (
	SET ARCH= ARM
	SET PREFIXFOLDER_ARCH=arm
)

call build-common.bat

Set ARCHIVE=gmock-1.7.0.zip
Set BUILD_DIR=%BUILD_ROOT%/gmock-1.7.0
Set PREFIX=%cd%/%PREFIXFOLDER_COMPILER%_%PREFIXFOLDER_ARCH%/gmock

echo "Compile GoogleMock with GoogleTest"

echo "Extracting GoogleMock with GoogleTest"
if not exist %BUILD_ROOT% exit /b
cd %BUILD_ROOT%

if exist %BUILD_DIR% RD /S /Q "%BUILD_DIR%"

winrar.exe x %EX_DIR%/%ARCHIVE%

if not exist %BUILD_DIR% exit /b

echo "Configuring GoogleMock with GoogleTest"
cd %BUILD_DIR%
"%CMake3%"\cmake . -DCMAKE_INSTALL_PREFIX=%PREFIX% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -G "%TOOLCHAIN%%ARCH%" .

echo "Building GoogleMock with GoogleTest"
MSBuild.exe gmock.sln /p:Configuration=Release

echo "Installing GoogleMock with GoogleTest"
mkdir "%PREFIX%"
mkdir "%PREFIX%/include"
mkdir "%PREFIX%/lib"
xcopy /S /Y "%BUILD_DIR%/gtest/include" "%PREFIX%/include" > NUL
xcopy /S /Y "%BUILD_DIR%/include" "%PREFIX%/include" > NUL

xcopy /S /Y "%BUILD_DIR%/gtest/Release" "%PREFIX%/lib" > NUL
xcopy /S /Y "%BUILD_DIR%/Release" "%PREFIX%/lib" > NUL

echo "Cleaning up"
cd %DEP_DIR%
RD /S /Q "%BUILD_DIR%"