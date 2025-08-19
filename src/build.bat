@echo off

set SRC=%CD%

set BUILD_DEBUG=1
set CFLAGS_COMMON="-nologo -W3"
set CFLAGS_DEBUG=" -Z7 -fsanitize=address"
set CFLAGS_INCLUDE=" -I%SRC% -DBUILD_DEBUG=%BUILD_DEBUG%"
set LFLAGS=-incremental:no -opt:ref

REM if %BUILD_DEBUG%==1 set CFLAGS=%CFLAGS% -Z7 -fsanitize=address
REM set CFLAGS=%CLFLAGS% -I%SRC% -DBUILD_DEBUG=%BUILD_DEBUG%
set CFLAGS=%CFLAGS_COMMON:~1,-1%%CFLAGS_DEBUG:~1,-1%%CFLAGS_INCLUDE:~1,-1%
echo %CFLAGS%

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set BUILD=%CD%

cl %CFLAGS% %SRC%\scratch\spectrogram.c /link %LFLAGS% /out:spectrogram.exe

:: build -> src
popd
