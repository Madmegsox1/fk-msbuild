#!/bin/sh
set -e

# "debug" or "release"
BUILD=debug

# Compiler & C++ version
CXX=clang++
STD=c++23

# Compiler flags
COMMON_CFLAGS="-std=$STD -Wall -Wextra -fno-omit-frame-pointer"
DEBUG_FLAGS="-g -O0 -DDEBUG_BUILD"
RELEASE_FLAGS="-O3 -DNDEBUG"

case "$BUILD" in
    debug)
        CFLAGS="$COMMON_CFLAGS $DEBUG_FLAGS"
        ;;
    release)
        CFLAGS="$COMMON_CFLAGS $RELEASE_FLAGS"
        ;;
    *)
        echo "Unknown BUILD mode: $BUILD"
        exit 1
        ;;
esac

# Platform agnostic

OS="$(uname -s)"

LDFLAGS=""
LIBS=""

# Change based on platform
case "$OS" in
    Linux)
        LIBS="$LIBS -lpthread"
        ;;
    Darwin) # macOS
        LIBS="$LIBS -lpthread"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        # Windows (POSIX layer)
        LIBS="$LIBS -lkernel32 -luser32"
        ;;
    *)
        echo "Unknown platform: $OS"
        ;;
esac

# Build process

start=$(date +%s.%N)

mkdir -p build

echo "Compiling ($BUILD, unity build)"
rm -f build/unity.cpp

(
    for f in $(find src -name "*.cpp"); do
        echo "#include \"../$f\""
    done
) > build/unity.cpp

# LSP database

rm -f compile_commands.json
echo "[" > compile_commands.json

echo "  Compiling unity.cpp..."
$CXX -MJ temp.json -c build/unity.cpp -o build/unity.o $CFLAGS

if [ -f temp.json ]; then
    cat temp.json >> compile_commands.json
    rm temp.json
fi

echo "]" >> compile_commands.json

# Linker

echo "Linking..."
$CXX build/unity.o -o build/app $LDFLAGS $LIBS

# End

end=$(date +%s.%N)
elapsed=$(echo "$end - $start" | bc)

echo "Build time: ${elapsed}s"
echo "Finished!"
