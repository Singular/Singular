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
(
    if [[ ! -d "$EXTERN_DIR/cddlib" ]]; then
        echo "Cloning cddlib..."
        git clone https://github.com/cddlib/cddlib.git "$EXTERN_DIR/cddlib"
    fi

    cd "$EXTERN_DIR/cddlib"
    
    if [[ ! -f configure ]]; then
        ./bootstrap
    fi

    if [[ ! -f Makefile ]]; then
        CPPFLAGS="-I$AUX_PREFIX/include" \
        LDFLAGS="-L$AUX_PREFIX/lib" \
        CFLAGS="-O2" \
        CXXFLAGS="-O2" \
        emconfigure ./configure \
            --with-gmp="$AUX_PREFIX" \
            --disable-shared \
            --prefix="$AUX_PREFIX"
    fi
    
    emmake make -j8
    emmake make install

    cd "$AUX_PREFIX/include"
    ln -sf cddlib/*.h .
    ln -sf cddmp.h cdd_mp.h
)

# --- NTL ---
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

        sed -e 's/^CC=gcc/CC=emcc -s NODERAWFS=1/' \
            -e 's/^WIZARD=on/WIZARD=off/' \
            makefile > makefile.patched
        mv makefile.patched makefile
    fi
    
    if ! emmake make -j8; then
        
        sed -e 's|^\t\./MakeDesc|\tchmod +x ./MakeDesc \&\& node ./MakeDesc|' \
            -e 's|^\t\./gen_gmp_aux|\tchmod +x ./gen_gmp_aux \&\& node ./gen_gmp_aux|' \
            -e 's|^\t\./gen_lip_gmp_aux|\tchmod +x ./gen_lip_gmp_aux \&\& node ./gen_lip_gmp_aux|' \
            -e 's|^\t\./gen_lip_gmp_aux|\tchmod +x ./gen_lip_gmp_aux \&\& node ./gen_lip_gmp_aux|' \
            makefile > makefile.patched
        mv makefile.patched makefile
        
        sed -i 's|if ./CheckFeatures|if node ./CheckFeatures|g' MakeCheckFeatures
        
        if [ -f MakeCheckThreads ]; then
            sed -i 's|./CheckThreads|node ./CheckThreads|g' MakeCheckThreads
        fi
        
        emmake make -j8
    fi

    emmake make install
    emranlib "$AUX_PREFIX/lib/libntl.a"
)

# --- NORMALIZ ---
(
    if [[ ! -d "$EXTERN_DIR/normaliz" ]]; then
        echo "Cloning Normaliz..."
        git clone https://github.com/Normaliz/Normaliz.git "$EXTERN_DIR/normaliz"
    fi

    cd "$EXTERN_DIR/normaliz"

    if [[ ! -f configure ]]; then
        echo "Bootstrapping Normaliz..."
        chmod +x bootstrap.sh
        ./bootstrap.sh
    fi

    if [[ ! -f Makefile ]]; then
        CPPFLAGS="-I$AUX_PREFIX/include" \
        LDFLAGS="-L$AUX_PREFIX/lib" \
        CFLAGS="-O2 -fexceptions" \
        CXXFLAGS="-O2 -fexceptions -std=c++14" \
        emconfigure ./configure \
            --build=i686-pc-linux-gnu \
            --host=wasm32-unknown-emscripten \
            --with-gmp="$AUX_PREFIX" \
            --with-flint="$AUX_PREFIX" \
            --disable-shared \
            --disable-openmp \
            --prefix="$AUX_PREFIX"
    fi
    
    emmake make -j8
    emmake make install
)

# --- TOPCOM ---
(
    mkdir -p "$AUX_BUILD/topcom"
    cd "$AUX_BUILD/topcom"
    
    if [[ ! -d "$EXTERN_DIR/topcom" ]]; then
        echo "Downloading TOPCOM source..."
        mkdir -p "$EXTERN_DIR/topcom"
        curl -sL "https://www.wm.uni-bayreuth.de/de/team/rambau_joerg/TOPCOM-Downloads/TOPCOM-1_2_0_eta.tgz" | tar -xz -C "$EXTERN_DIR/topcom" --strip-components=1
    fi

    if [[ ! -f Makefile ]]; then
        CPPFLAGS="-I$AUX_PREFIX/include" \
        LDFLAGS="-L$AUX_PREFIX/lib -static -s USE_PTHREADS=0" \
        CFLAGS="-O2 -fexceptions -s USE_PTHREADS=0" \
        CXXFLAGS="-O2 -fexceptions -std=c++17 -s USE_PTHREADS=0" \
        emconfigure "$EXTERN_DIR/topcom/configure" \
            --build=i686-pc-linux-gnu \
            --host=wasm32-unknown-emscripten \
            --disable-shared \
            --enable-static \
            --prefix="$AUX_PREFIX"
            
        find . -type f -name "Makefile" -exec sed -i 's/ -pthread / /g; s/ -pthread$/ /g' {} +
        if [ -f libtool ]; then 
            sed -i 's/ -pthread / /g' libtool
            sed -i 's/"-pthread /"/g' libtool
            sed -i 's/ -pthread"/"/g' libtool
        fi
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
    --with-topcom="$AUX_PREFIX" \
    --disable-shared \
    --enable-static \
    --without-pic \
    --without-readline \
    --enable-gfanlib \
    --disable-polymake \
    --disable-pthreads \
    --disable-omalloc \
    --enable-p-procs-static \
    --disable-p-procs-dynamic \
    --with-builtinmodules=syzextra,gfanlib,freealgebra,subsets,cohomo,loctriv,customstd,partialgb,sispasm \
    CXX="em++ -fexceptions" \
    CC="emcc -fexceptions" \
    CXXFLAGS="-O2 -fexceptions -D_GNU_SOURCE -std=c++14 -I$AUX_PREFIX/include -I$AUX_PREFIX/include/cddlib" \
    CFLAGS="-O2 -fexceptions -D_GNU_SOURCE -I$AUX_PREFIX/include -I$AUX_PREFIX/include/cddlib" \
    LDFLAGS="-L$AUX_PREFIX/lib -L$BASEDIR -lwasm_patch -fexceptions -s ASYNCIFY=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=0 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -O2"

emmake make -j8

echo "Building static modules..."

for mod in syzextra gfanlib freealgebra subsets cohomo loctriv customstd partialgb sispasm; do
    echo "Building module: $mod"
    emmake make -C "Singular/dyn_modules/$mod" -j8
done

cp "$BASEDIR/emscripten/wasm_patch.c" "$BASEDIR/Singular"
cd Singular

MODULE_LIBS="
    dyn_modules/syzextra/.libs/libsyzextra.a \
    dyn_modules/gfanlib/.libs/libgfanlib.a \
    dyn_modules/freealgebra/.libs/libfreealgebra.a \
    dyn_modules/subsets/.libs/libsubsets.a \
    dyn_modules/cohomo/.libs/libcohomo.a \
    dyn_modules/loctriv/.libs/libloctriv.a \
    dyn_modules/customstd/.libs/libcustomstd.a \
    dyn_modules/partialgb/.libs/libpartialgb.a \
    dyn_modules/sispasm/.libs/libsispasm.a"

em++ wasm_patch.c tesths.o utils.o \
    -Wl,--start-group \
    ./.libs/libSingular.a \
    ../libpolys/polys/.libs/libpolys.a \
    ../factory/.libs/libfactory.a \
    ../resources/.libs/libsingular_resources.a \
    $MODULE_LIBS \
    -Wl,--end-group \
    -L"$AUX_PREFIX/lib" \
    -lTOPCOM -lnormaliz -lflint -lmpfr -lcddgmp -lntl -lgmp\
    -s ASYNCIFY=1 \
    -s TOTAL_STACK=64MB \
    -s INITIAL_MEMORY=1024MB \
    -s ALLOW_MEMORY_GROWTH=1 \
    -fexceptions \
    -O2 \
    --preload-file LIB@/LIB \
    --preload-file ../doc@/info \
    -o Singular.html

echo "Build complete."