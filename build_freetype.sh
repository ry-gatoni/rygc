#!/usr/bin/env bash

ROOT=$PWD
FREETYPE_ROOT=$ROOT/src/third_party/freetype-2.13.3

IFLAGS="-I$ROOT/src/third_party/freetype_custom"
IFLAGS+=" -I$FREETYPE_ROOT/include"

FILES="$FREETYPE_ROOT/src/base/ftsystem.c"
FILES+=" $FREETYPE_ROOT/src/base/ftinit.c"
FILES+=" $FREETYPE_ROOT/src/base/ftdebug.c"
FILES+=" $FREETYPE_ROOT/src/base/ftbase.c"
FILES+=" $FREETYPE_ROOT/src/base/ftbbox.c"
FILES+=" $FREETYPE_ROOT/src/base/ftglyph.c"
FILES+=" $FREETYPE_ROOT/src/base/ftbitmap.c"
FILES+=" $FREETYPE_ROOT/src/sfnt/sfnt.c"
FILES+=" $FREETYPE_ROOT/src/truetype/truetype.c"
FILES+=" $FREETYPE_ROOT/src/smooth/smooth.c"
FILES+=" $FREETYPE_ROOT/src/psnames/psnames.c"

mkdir -p build
pushd build > /dev/null

rm *.o
clang -c -g -DFT2_BUILD_LIBRARY $IFLAGS $FILES
ar rcs libfreetype.a *.o

popd > /dev/null # rygc/build -> rygc
