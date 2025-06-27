#!/usr/bin/env bash

BUILD_DEBUG=1

## TODO: it would be nice to enable -Wconversion, but it produces far too many warnings to be useful
CFLAGS="-Wall -Wextra -Wshadow -Wno-initializer-overrides -Wno-unused-function"
if [[ $BUILD_DEBUG == 1 ]]; then
    CFLAGS+=" -g -fsanitize=address"
fi
CFLAGS+=" -DBUILD_DEBUG=$BUILD_DEBUG"
if [[ "$OS_TYPE" == "darwin"* ]]; then
    echo ""
elif [[ "$OS_TYPE" == "linux-gnu"* ]]; then
    echo ""
fi
CFLAGS+=" -fvisibility=hidden"

SRC=$PWD
mkdir -p ../build
pushd ../build > /dev/null

clang $CFLAGS $SRC/wayland_codegen.c -o wayland_codegen
clang $CFLAGS $SRC/wayland_window_test.c -o wayland_window_test

popd > /dev/null # ./build -> ./src

#clang_common="-I../src/ -I/usr/include/freetype2/ -I../local/ -g -DBUILD_GIT_HASH=\"$git_hash\" -DBUILD_GIT_HASH_FULL=\"$git_hash_full\" -Wno-unknown-warning-option -fdiagnostics-absolute-paths -Wall -Wno-missing-braces -Wno-unused-function -Wno-writable-strings -Wno-unused-value -Wno-unused-variable -Wno-unused-local-typedef -Wno-deprecated-register -Wno-deprecated-declarations -Wno-unused-but-set-variable -Wno-single-bit-bitfield-constant-conversion -Wno-compare-distinct-pointer-types -Wno-initializer-overrides -Wno-incompatible-pointer-types-discards-qualifiers -Wno-for-loop-analysis -Xclang -flto-visibility-public-std -D_USE_MATH_DEFINES -Dstrdup=_strdup -Dgnu_printf=printf"
