#!/usr/bin/env bash

BUILD_DEBUG=1

## TODO: it would be nice to enable -Wconversion, but it produces far too many warnings to be useful
CFLAGS="-Wall -Wextra -Wshadow -Wno-unused-function -Wno-initializer-overrides -Wno-unused-local-typedef"
if [[ $BUILD_DEBUG == 1 ]]; then
    CFLAGS+=" -g -fsanitize=address"
fi
CFLAGS+=" -DBUILD_DEBUG=$BUILD_DEBUG"
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo ""
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo ""
fi
CFLAGS+=" -fvisibility=hidden"

SRC=$PWD
CFLAGS+=" -I$SRC"
CFLAGS+=" -I$SRC/third_party/freetype-2.13.3/include"

mkdir -p ../build
pushd ../build > /dev/null

BUILD=$PWD
CFLAGS+=" -L$BUILD"

#clang $CFLAGS $SRC/wayland_codegen.c -o wayland_codegen
#clang $CFLAGS $SRC/scratch/jack_test.c -ljack -o jack_test
#clang $CFLAGS $SRC/scratch/wayland_window_test.c -lxkbcommon -lEGL -lGL -o wayland_window_test
#clang $CFLAGS $SRC/scratch/freetype_test.c -lfreetype -lxkbcommon -lEGL -lGL -o freetype_test
clang $CFLAGS $SRC/scratch/spectrogram.c -lxkbcommon -lEGL -lGL -lfreetype -ljack -o spectrogram
#clang $CFLAGS $SRC/tests/fft_test.c -o fft_test

popd > /dev/null # ./build -> ./src

#clang_common="-I../src/ -I/usr/include/freetype2/ -I../local/ -g -DBUILD_GIT_HASH=\"$git_hash\" -DBUILD_GIT_HASH_FULL=\"$git_hash_full\" -Wno-unknown-warning-option -fdiagnostics-absolute-paths -Wall -Wno-missing-braces -Wno-unused-function -Wno-writable-strings -Wno-unused-value -Wno-unused-variable -Wno-unused-local-typedef -Wno-deprecated-register -Wno-deprecated-declarations -Wno-unused-but-set-variable -Wno-single-bit-bitfield-constant-conversion -Wno-compare-distinct-pointer-types -Wno-initializer-overrides -Wno-incompatible-pointer-types-discards-qualifiers -Wno-for-loop-analysis -Xclang -flto-visibility-public-std -D_USE_MATH_DEFINES -Dstrdup=_strdup -Dgnu_printf=printf"
