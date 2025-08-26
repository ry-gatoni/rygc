@echo off

set SRC=%CD%

:: NOTE: in debug mode, the llvm address santizer is enabled, which means
:: `clang_rt.asan_dbg_dynamic-x86_64.dll`, or a similarly named file, must be on
:: the system PATH in order to run the programs this script compiles
set BUILD_DEBUG=1
set CFLAGS_COMMON="-nologo -W3"
set CFLAGS_DEBUG=" -Z7 -fsanitize=address"
set CFLAGS_INCLUDE=" -I%SRC% -DBUILD_DEBUG=%BUILD_DEBUG%"
set LFLAGS=-incremental:no -opt:ref

REM if %BUILD_DEBUG%==1 set CFLAGS=%CFLAGS% -Z7 -fsanitize=address
if %BUILD_DEBUG%==1 (
  set CFLAGS=%CFLAGS_COMMON:~1,-1%%CFLAGS_DEBUG:~1,-1%%CFLAGS_INCLUDE:~1,-1%
) else (
  set CFLAGS=%CFLAGS_COMMON:~1,-1%%CFLAGS_INCLUDE:~1,-1%
)
echo %CFLAGS%

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set BUILD=%CD%

cl %CFLAGS% %SRC%\scratch\spectrogram.c /link %LFLAGS% /out:spectrogram.exe
cl %CFLAGS% %SRC%\scratch\win32_window_test.c /link %LFLAGS% /out:win32_window_test.exe

:: build -> src
popd
