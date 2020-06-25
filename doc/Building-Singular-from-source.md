Building Singular from Sources {#build_sources_page}
==============================

These are the generic installation instructions for experts, see

[Step by Step Installation Instructions for Singular](Step by Step Installation Instructions for Singular)

for step by step instructions.

# Build the development branch of Singular as follows:

1. as prerequisite autotools (>=2.62), [gmp](http://ftp.gnu.org/gnu/gmp/) (>= 4.2), are needed, furthermore we recommend to use [NTL](http://www.shoup.net/ntl/) (>= 5.0) configured with NTL_GMP_LIP=on (for further details see [NTL Documentation](http://www.shoup.net/ntl/doc/tour-gmp.html)), [FLINT](http://www.flintlib.org/) (>=2.4) which depends on [MPFR](http://www.mpfr.org/mpfr-current/) (>=3.0.0)  , and readline
2. get the sources with `git clone -b spielwiese git://github.com/Singular/Singular.git <directory_name>`
3. run `<abs_directory_name_from_above>/autogen.sh` (formerly known as `for_Hans_with_love.sh`) from the root directory
4. create and switch to your temporary build directory. Do not build Singular in the source folder itself!
5. run `<abs_directory_name_from_above>/configure --prefix=<destination path>` (builds release version)
6. `make` (or `make -jN` where N is the number of the your CPU cores + 1)
7. `make -jN check` (where N is as above) will build and run simple unit-tests for most units (optional but encouraged)
8. `make install`
9. developers are also encouraged to run `make distcheck`

## Static variants of Singular may be built using the following configure flags:

* `--with-readline=static` (embed readline)
* `--disable-shared --enable-static --without-pic` (totally disable dynamic linkage)
* `--enable-p-procs-static --disable-p-procs-dynamic` (builds-in the the generated p_Procs functions)

Moreover modules may be built-in as follows:
* `--with-builtinmodules=A,M,...` means that the listed modules are to be built-in. Possible internal built-ins are:
  * `syzextra`  (default if no modules were listed)
  * `gfanlib`, `polymake`, `pyobject`, `singmathic` (optional)
  * `staticdemo`, `bigintm` (demos)
Note that if you want to built-in an extra module,m say `M` you will have to do the following:
  * built `M`, generate a valid `M.la` file and place them in `$BUILTDIR/Singular/dyn_modules/M/`
    * one can also set `BUILTIN_LIBS=LD_FLAGS_FOR_EMBEDDING_M`
  * In configure time explicitly set `--with-builtinmodules=...,M,...`
  * built-ins are required to provide `extern "C" int SI_MOD_INIT0(M)(SModulFunctions*)` but no `mod_init` symbols!

Note: modules are not required to be static...

## Debug version of Singular

* use the following configure arguments: `--enable-debug --disable-optimizationflags`

# Test the Spielwiese Singular as follows:

* the test-suites from Tst/ can be run, for example, as follows:
  1. `cd Tst/`
  2. `ln -s <abs_destination path>/bin/Singular`
  3. after the above symbolic-linking one can simply run `./regress.cmd something.{lst,tst}`, where .lst files are just lists of tests, which are .tst files

# Build the old ```master``` Singular as follows:

1. get the sources with `git clone -b trunk git://github.com/Singular/Singular.git <directory_name>`
2. `cd <directory_name_from_above>`
3. run `./configure` from the root directory
4. `make install` (or `make -jN install`, where N as above), and _yes_ it should really be `install`!
5. everything will be put to the newly created directory "$Architecture-$OS/" (e.g. x86_64-Linux, ix86-Linux etc.)

# Coverage testing with `lcov` (thanks to Jakob Kroeker)

1. make sure that `gcov` is present and install a recent lcov (>= 1.10)
2. configure and build `Spielwiese` (IN SOURCE TREE!) together with the following FLAGS:
```
LDFLAGS+="-lgcov"
CFLAGS+="-fprofile-arcs -ftest-coverage -g"
CXXFLAGS+="-fprofile-arcs -ftest-coverage -g"
```
3. if necessary clean-up lcov-state-files with: `lcov -z -d . `
4. run tests (e.g. `Tst/regress.cmd -s Singular/Singular Tst/Short.lst`)
5. collect coverage data into `coverage.cov` with `lcov -c -d . -o coverage.cov`
6. optionally remove unnecessary directories: `lcov -r coverage.cov '/usr/include/ *' > coverage2.cov`
7. generate HTML overview pages under `GenHtmlDir/` with `genhtml -o GenHtmlDir coverage.cov`

For instance, a recent test coverage for Singular
(_static 64-bit build with optimization, gfanlib, pyobjects, countedref using NTL, MPIR (as GMP), FLINT, readline, git id: e86e21bd*, on Linux, with GCC 4.8.2_20131219)
on our testsuite (with _~2420 tests_) and unittests is:
* [Lines: 65.6 %, Functions: 58.5 %](http://www.mathematik.uni-kl.de/~motsak/lcov/)
* [Lines: 67.6 %, Functions: 57.4 %](http://www.mathematik.uni-kl.de/~motsak/lcov1/)


See also [**Use gcov and lcov to know your test coverage**](http://qiaomuf.wordpress.com/2011/05/26/use-gcov-and-lcov-to-know-your-test-coverage/)
