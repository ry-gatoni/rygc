@echo off

set SRC=%CD%

set BUILD_DEBUG=1
set CFLAGS=-nologo -W3
set LFLAGS=-incremental:no -opt:ref

if %BUILD_DEBUG%==1 set CFLAGS=%CFLAGS% -Z7 -fsanitize=address
set CFLAGS=%CLFLAGS% -I%SRC% -DBUILD_DEBUG=%BUILD_DEBUG%

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set BUILD=%CD%

cl %CFLAGS% %SRC%\scratch\spectrogram.c /link %LFLAGS% /out:spectrogram.exe

:: build -> src
popd
