#!/usr/bin/env bash
set -eux

BASEDIR="$(pwd)"

if ! command -v emmake &> /dev/null; then
    echo "Please install and source Emscripten."
    echo "See https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

AUX_BUILD="$BASEDIR/emscripten/build"
AUX_PREFIX="$BASEDIR/emscripten/install"
EXTERN_DIR="$BASEDIR/emscripten/extern"

mkdir -p "$AUX_BUILD"
mkdir -p "$AUX_PREFIX"
mkdir -p "$EXTERN_DIR"

# Generate autotools scripts if missing
if [[ ! -f ./configure ]]; then
    ./autogen.sh
fi

# Build Dependencies to Wasm

# --- GMP ---
(
    mkdir -p "$AUX_BUILD/gmp"
    cd "$AUX_BUILD/gmp"
    if [[ ! -d "$EXTERN_DIR/gmp" ]]; then
        echo "Downloading GMP source..."
        hg clone https://gmplib.org/repo/gmp/ "$EXTERN_DIR/gmp"
        cd "$EXTERN_DIR/gmp" && ./.bootstrap && cd -
    fi
    if [[ ! -f config.status ]]; then
        # GMP requires specific flags to disable assembly for Wasm
        CC_FOR_BUILD=/usr/bin/gcc ABI=standard \
        emconfigure "$EXTERN_DIR/gmp/configure" \
        --build i686-pc-linux-gnu --host none \
        --disable-assembly --enable-cxx \
        --prefix="$AUX_PREFIX"
    fi
    emmake make -j8
    emmake make install
)

# --- MPFR ---
(
    mkdir -p "$AUX_BUILD/mpfr"
    cd "$AUX_BUILD/mpfr"
    if [[ ! -d "$EXTERN_DIR/mpfr" ]]; then
        echo "Downloading MPFR source..."
        git clone https://gitlab.inria.fr/mpfr/mpfr.git "$EXTERN_DIR/mpfr"
        cd "$EXTERN_DIR/mpfr" && ./autogen.sh && cd -
    fi
    if [[ ! -f config.status ]]; then
        emconfigure "$EXTERN_DIR/mpfr/configure" \
        --build i686-pc-linux-gnu --host none \
        --with-gmp="$AUX_PREFIX" \
        --disable-shared \
        --prefix="$AUX_PREFIX"
    fi
    emmake make -j8
    emmake make install
)

# --- FLINT ---
(
    mkdir -p "$AUX_BUILD/flint"
    cd "$AUX_BUILD/flint"
    if [[ ! -d "$EXTERN_DIR/flint2" ]]; then
        echo "Cloning Flint..."
        git clone --depth 1 https://github.com/wbhart/flint2.git "$EXTERN_DIR/flint2"
        cd "$EXTERN_DIR/flint2" && ./bootstrap.sh && cd -
    fi
    if [[ ! -f Makefile ]]; then
        emconfigure "$EXTERN_DIR/flint2/configure" \
        --build=i686-pc-linux-gnu \
        --host=wasm32-unknown-emscripten \
        --with-gmp="$AUX_PREFIX" \
        --with-mpfr="$AUX_PREFIX" \
        --disable-shared \
        --disable-assembly \
        --prefix="$AUX_PREFIX"
    fi
    emmake make -j8
    emmake make install
)

# --- CDDLIB ---
#(
#    mkdir -p "$AUX_BUILD/cddlib"
#    cd "$AUX_BUILD/cddlib"
#    if [[ ! -d "$EXTERN_DIR/cddlib" ]]; then
#        echo "Cloning cddlib..."
#        git clone https://github.com/cddlib/cddlib.git "$EXTERN_DIR/cddlib"
#        cd "$EXTERN_DIR/cddlib" && ./bootstrap && cd -
#    fi
#    if [[ ! -f Makefile ]]; then
#        emconfigure "$EXTERN_DIR/cddlib/configure" \
#        --build=i686-pc-linux-gnu \
#        --host=wasm32-unknown-emscripten \
#        --with-gmp="$AUX_PREFIX" \
#        --disable-shared \
#        --prefix="$AUX_PREFIX"
#    fi
#    
#    emmake make -j8
#    emmake make install
#)


cd "$BASEDIR"
#    cddlib does not work for unkonw issue reason
#    to enable gfanlib, you need to add the flag --with-cdd="$AUX_PREFIX" \

if [[ ! -f Makefile ]] || ! grep 'emcc' Makefile > /dev/null; then
    emconfigure ./configure \
    ac_cv_func_qsort_r=no \
    --host=wasm32-unknown-emscripten \
    --with-gmp="$AUX_PREFIX" \
    --with-mpfr="$AUX_PREFIX" \
    --with-flint="$AUX_PREFIX" \
    --disable-shared \
    --without-readline \
    --disable-gfanlib \
    --disable-polymake \
    --disable-pthreads \
    --disable-omalloc \
    CXXFLAGS="-O2 -D_GNU_SOURCE" \
    CFLAGS="-O2 -D_GNU_SOURCE" \
    LDFLAGS="-s ASYNCIFY=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=0 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -O2"
fi

emmake make -j8

cp "$BASEDIR/emscripten/dummy.c" "$BASEDIR/Singular"
cd Singular

# ERROR_ON_UNDEFINED_SYMBOLS=0, ASSERTIONS=1, fexceptions and -g is an attempt to make lib loading works. It failed though.
em++ wasm_patch.c tesths.o utils.o \
    ./.libs/libSingular.a \
    ../libpolys/polys/.libs/libpolys.a \
    ../factory/.libs/libfactory.a \
    ../resources/.libs/libsingular_resources.a \
    -L"$AUX_PREFIX/lib" -lflint -lmpfr -lgmp \
    -s ASYNCIFY=1 \
    -s TOTAL_STACK=32MB \
    -s INITIAL_MEMORY=1024MB \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
    -s ASSERTIONS=1 \
    -fexceptions \
    -g \
    -O2 \
    --preload-file LIB@/LIB \
    --preload-file ../doc@/info \
    -o Singular.html

echo "Build complete."