@echo off

SET ARCH=Visual Studio 12
SET PREFIXFOLDER=x86
IF [%1] == [64] (
	SET ARCH=Visual Studio 12 Win64
	SET PREFIXFOLDER=x64
)
IF [%1] == [32] (
	SET ARCH=Visual Studio 12
	SET PREFIXFOLDER=x86
)

call build-common.bat

Set ARCHIVE=gmock-1.7.0.zip
Set BUILD_DIR=%BUILD_ROOT%/gmock-1.7.0
Set PREFIX=%cd%/%PREFIXFOLDER%/gmock

echo "Compile GoogleMock with GoogleTest"

echo "Extracting GoogleMock with GoogleTest"
if not exist %BUILD_ROOT% exit /b
cd %BUILD_ROOT%

if exist %BUILD_DIR% RD /S /Q "%BUILD_DIR%"

winrar.exe x %EX_DIR%/%ARCHIVE%

if not exist %BUILD_DIR% exit /b

echo "Configuring GoogleMock with GoogleTest"
cd %BUILD_DIR%
cmake . -DCMAKE_INSTALL_PREFIX=%PREFIX% -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -G "%ARCH%" . > NUL

echo "Building GoogleMock with GoogleTest"
MSBuild.exe gmock.sln /p:Configuration=Release > NUL

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