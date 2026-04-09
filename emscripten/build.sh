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
# Emscripten compilation of cddlib does not produce cdd_f.h, this ugly patch script generate it manually.  
(
    mkdir -p "$AUX_BUILD/cddlib"
    cd "$AUX_BUILD/cddlib"
    if [[ ! -d "$EXTERN_DIR/cddlib" ]]; then
        echo "Cloning cddlib..."
        git clone https://github.com/cddlib/cddlib.git "$EXTERN_DIR/cddlib"
        cd "$EXTERN_DIR/cddlib" && ./bootstrap && cd -
    fi

    generate_f_header() {
        local in_file="$1"
        local out_file="$2"
        sed -e 's/dd_/ddf_/g' \
            -e 's/cddf_/cdd_/g' \
            -e 's/mytype/myfloat/g' \
            -e 's/#include "cdd.h"/#include "cdd_f.h"/' \
            -e 's/#include "cddtypes.h"/#include "cddtypes_f.h"/' \
            -e 's/#include "cddmp.h"/#include "cddmp_f.h"/' \
            -e 's/__CDD_H/__CDD_HF/' \
            -e 's/__CDD_HFF/__CDD_HF/' \
            -e 's/__CDDMP_H/_CDDMP_HF/' \
            -e 's/__CDDTYPES_H/_CDDTYPES_HF/' \
            -e 's/GMPRATIONAL/ddf_GMPRATIONAL/g' \
            -e 's/ARITHMETIC/ddf_ARITHMETIC/g' \
            -e 's/CDOUBLE/ddf_CDOUBLE/g' \
            "$in_file" | awk -v name="$in_file" 'BEGIN{print "/* generated automatically from " name " */"}1' > "$out_file"
    }

    cd "$EXTERN_DIR/cddlib/lib-src"
    generate_f_header cdd.h cdd_f.h
    generate_f_header cddmp.h cddmp_f.h
    generate_f_header cddtypes.h cddtypes_f.h
    cd "$AUX_BUILD/cddlib"

    if [[ ! -f Makefile ]]; then
        export CPPFLAGS="-I$AUX_PREFIX/include"
        export LDFLAGS="-L$AUX_PREFIX/lib"
        export CFLAGS="-O2"
        export CXXFLAGS="-O2"
        emconfigure "$EXTERN_DIR/cddlib/configure" \
        --build=i686-pc-linux-gnu \
        --host=wasm32-unknown-emscripten \
        --with-gmp="$AUX_PREFIX" \
        --disable-shared \
        --prefix="$AUX_PREFIX"
    fi
    
    emmake make -j8
    emmake make install

    cp "$EXTERN_DIR/cddlib/lib-src"/*_f.h "$AUX_PREFIX/include/cddlib/"

    cd "$AUX_PREFIX/include"
    ln -sf cddlib/*.h .
    
    ln -sf cddmp.h cdd_mp.h
)

# --- NTL ---
# This ugly patch script is intended to fix the cross-compilation issue of ntl.
(
    mkdir -p "$AUX_BUILD/ntl"
    cd "$AUX_BUILD/ntl"
    if [[ ! -d "$EXTERN_DIR/ntl" ]]; then
        echo "Cloning NTL..."
        git clone https://github.com/libntl/ntl.git "$EXTERN_DIR/ntl"
    fi
    cd "$EXTERN_DIR/ntl/src"
    if [[ ! -f makefile ]]; then
        emconfigure ./configure \
        CXX="em++" \
        CXXFLAGS="-O2 -fexceptions -s WASM=1 -s NODERAWFS=1" \
        PREFIX="$AUX_PREFIX" \
        GMP_PREFIX="$AUX_PREFIX" \
        NTL_GMP_LIP=on \
        NTL_STD_CXX14=on \
        SHARED=off \
        NATIVE=off \
        TUNE=generic \
        NTL_THREADS=off

        NODE_BIN=$(command -v node || echo "node")
        
        em++ -I../include -I. -O2 -fexceptions -s WASM=1 -s NODERAWFS=1 -c MakeDescAux.cpp
        em++ -I../include -I. -O2 -fexceptions -s WASM=1 -s NODERAWFS=1 -o MakeDesc.js MakeDesc.cpp MakeDescAux.o -lm
        $NODE_BIN ./MakeDesc.js
        
        cp mach_desc.h ../include/NTL/mach_desc.h
        cp mach_desc.h mach_desc_safe.h

        em++ -I../include -I. -O2 -fexceptions -s WASM=1 -s NODERAWFS=1 -I"$AUX_PREFIX/include" -o gen_gmp_aux.js gen_gmp_aux.cpp -L"$AUX_PREFIX/lib" -lgmp
        $NODE_BIN ./gen_gmp_aux.js > ../include/NTL/gmp_aux_safe.h

        cp "$BASEDIR/emscripten/GetTime.cpp" .
        cp "$BASEDIR/emscripten/GetPID.cpp" .
        cat "$BASEDIR/emscripten/ntl_config_append.h" >> ../include/NTL/config.h

        sed -e 's|^\t\./MakeDesc|\tcp mach_desc_safe.h mach_desc.h|g' \
            -e 's|^\t\./gen_gmp_aux.*|\tcp ../include/NTL/gmp_aux_safe.h ../include/NTL/gmp_aux.h|g' \
            -e 's|^\t\./TestGetTime|\ttrue|g' \
            -e 's|^\t\./TestGetPID|\ttrue|g' \
            -e 's|^\tsh MakeCheckFeatures.*|\ttrue|g' \
            -e 's|^\t\./CheckFeatures|\ttrue|g' \
            -e 's|^\t\./CheckThreads|\ttrue|g' \
            makefile > makefile.patched
        
        mv makefile.patched makefile
    fi
    
    emmake make -j8
    emmake make install
    emranlib "$AUX_PREFIX/lib/libntl.a"
)

# --- NORMALIZ ---
(
    mkdir -p "$AUX_BUILD/normaliz"
    cd "$AUX_BUILD/normaliz"
    
    if [[ ! -d "$EXTERN_DIR/normaliz" ]]; then
        echo "Cloning Normaliz..."
        git clone https://github.com/Normaliz/Normaliz.git "$EXTERN_DIR/normaliz"
    fi

    if [[ ! -f "$EXTERN_DIR/normaliz/configure" ]]; then
        echo "Bootstrapping Normaliz..."
        cd "$EXTERN_DIR/normaliz"
        chmod +x bootstrap.sh
        ./bootstrap.sh
        cd -
    fi

    if [[ ! -f Makefile ]]; then
        emconfigure "$EXTERN_DIR/normaliz/configure" \
        --build=i686-pc-linux-gnu \
        --host=wasm32-unknown-emscripten \
        --with-gmp="$AUX_PREFIX" \
        --with-flint="$AUX_PREFIX" \
        --disable-shared \
        --disable-openmp \
        --prefix="$AUX_PREFIX" \
        CPPFLAGS="-I$AUX_PREFIX/include" \
        LDFLAGS="-L$AUX_PREFIX/lib"
    fi
    
    emmake make -j8
    emmake make install
)

cd "$BASEDIR"

export EMCC_CFLAGS="-fexceptions"

emcc -c "$BASEDIR/emscripten/wasm_patch.c" -o "$BASEDIR/wasm_patch.o"
emar rcs "$BASEDIR/libwasm_patch.a" "$BASEDIR/wasm_patch.o"

emconfigure ./configure \
    ac_cv_func_qsort_r=no \
    --host=wasm32-unknown-emscripten \
    --with-gmp="$AUX_PREFIX" \
    --with-mpfr="$AUX_PREFIX" \
    --with-flint="$AUX_PREFIX" \
    --with-cdd="$AUX_PREFIX" \
    --with-ntl="$AUX_PREFIX" \
    --with-normaliz="$AUX_PREFIX" \
    --disable-shared \
    --without-readline \
    --enable-gfanlib \
    --disable-polymake \
    --disable-pthreads \
    --disable-omalloc \
    --with-builtinmodules=syzextra,gfanlib \
    CXX="em++ -fexceptions" \
    CC="emcc -fexceptions" \
    CXXFLAGS="-O2 -fexceptions -D_GNU_SOURCE -I$AUX_PREFIX/include -I$AUX_PREFIX/include/cddlib" \
    CFLAGS="-O2 -fexceptions -D_GNU_SOURCE -I$AUX_PREFIX/include -I$AUX_PREFIX/include/cddlib" \
    LDFLAGS="-L$AUX_PREFIX/lib -L$BASEDIR -lwasm_patch -fexceptions -s ASYNCIFY=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=0 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -O2"


emmake make -j8

cp "$BASEDIR/emscripten/wasm_patch.c" "$BASEDIR/Singular"
cd Singular

em++ wasm_patch.c tesths.o utils.o \
    ./.libs/libSingular.a \
    ../libpolys/polys/.libs/libpolys.a \
    ../factory/.libs/libfactory.a \
    ../resources/.libs/libsingular_resources.a \
    -L"$AUX_PREFIX/lib" -lnormaliz -lflint -lmpfr -lcddgmp -lntl -lgmp \
    -s ASYNCIFY=1 \
    -s TOTAL_STACK=32MB \
    -s INITIAL_MEMORY=1024MB \
    -s ALLOW_MEMORY_GROWTH=1 \
    -fexceptions \
    -O2 \
    --preload-file LIB@/LIB \
    --preload-file ../doc@/info \
    -o Singular.html

echo "Build complete."
