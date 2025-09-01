@echo off

set ROOT=%CD%
set FREETYPE_ROOT=%ROOT%\src\third_party\freetype-2.13.3

set IFLAGS=
set IFLAGS=%IFLAGS% -I%ROOT%\src\third_party\freetype_custom
set IFLAGS=%IFLAGS% -I%FREETYPE_ROOT%\include

set FILES=
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftsystem.c
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftinit.c
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftdebug.c
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftbase.c
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftbbox.c
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftglyph.c
set FILES=%FILES% %FREETYPE_ROOT%\src\base\ftbitmap.c
set FILES=%FILES% %FREETYPE_ROOT%\src\sfnt\sfnt.c
set FILES=%FILES% %FREETYPE_ROOT%\src\truetype\truetype.c
set FILES=%FILES% %FREETYPE_ROOT%\src\smooth\smooth.c
set FILES=%FILES% %FREETYPE_ROOT%\src\psnames\psnames.c

if not exist %ROOT%\build mkdir %ROOT%\build
pushd %ROOT%\build

if not exist %ROOT%\build\freetype mkdir %ROOT%\build\freetype
pushd %ROOT%\build\freetype

del *.obj
cl -c -nologo -Z7 -DFT2_BUILD_LIBRARY %IFLAGS% %FILES%
lib -nologo -OUT:"freetype.lib" *.obj

:: build\freetype -> build
popd

copy freetype\freetype.lib freetype.lib

:: build -> root
popd
