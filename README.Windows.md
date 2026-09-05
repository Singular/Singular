# Building Singular natively on Windows

Singular can be built as a native 64-bit Windows console application with the
MSYS2 UCRT64 toolchain.  MSYS2 supplies the build shell and packages, but the
resulting `Singular.exe` is a MinGW/UCRT program: it does not depend on
`msys-2.0.dll` or Cygwin's `cygwin1.dll`.

## Prerequisites

Install [MSYS2](https://www.msys2.org/) and open its **UCRT64** terminal.  First
update MSYS2 as directed by its installer (close and reopen the terminal if the
update asks you to), then install the build dependencies:

```sh
pacman -Syu
pacman -S --needed \
  base-devel autoconf automake bison flex libtool perl \
  mingw-w64-ucrt-x86_64-toolchain \
  mingw-w64-ucrt-x86_64-autotools \
  mingw-w64-ucrt-x86_64-flint \
  mingw-w64-ucrt-x86_64-gmp \
  mingw-w64-ucrt-x86_64-ntl \
  mingw-w64-ucrt-x86_64-ntldd \
  mingw-w64-ucrt-x86_64-readline
```

Confirm that the UCRT64 compiler is active:

```sh
which gcc
gcc -dumpmachine
```

The first command should name `/ucrt64/bin/gcc`, and the target should contain
`mingw32`.

## Build and install

Run the following in the UCRT64 terminal from a fresh source checkout:

```sh
./autogen.sh
mkdir build-windows
cd build-windows

../configure \
  --prefix="$HOME/singular-native" \
  --disable-shared \
  --enable-static \
  --disable-gfanlib \
  --with-ntl=yes \
  --with-flint=yes \
  --with-readline=static \
  --without-python \
  --disable-python

make -j2
make install
"$HOME/singular-native/bin/Singular.exe"
```

Use `cygpath -w "$HOME/singular-native"` to print the corresponding Windows
installation path.  A manually built tree uses native dependency DLLs from
`/ucrt64/bin`, so either keep that directory on `PATH` while developing or copy
the complete DLL dependency closure next to `Singular.exe`.

End users do **not** need MSYS2 or Cygwin.  The GitHub Actions workflow creates
the `singular-windows-ucrt64` portable artifact with every non-system DLL next
to the executable that needs it.  Installation is just:

1. Download and extract the artifact or release ZIP to a directory of your
   choice.
2. Run `bin\Singular.exe` from PowerShell, `cmd.exe`, or Windows Terminal.

No administrator access, registry changes, MSYS2 installation, or Cygwin
installation is required.  UCRT is part of supported Windows 10 and Windows 11
systems; the archive carries the MinGW, FLINT, NTL, readline, and other native
runtime DLLs.  It also contains the standard Singular libraries and relevant
third-party license files.

## Current native-Windows scope

This initial port builds a static Singular core with the required arithmetic
and interactive dependencies:

- `Singular.exe`, the standard libraries, headers, and static project libraries
  are built and installed.
- Polynomial procedure modules are linked statically.
- FLINT, NTL, and readline are required; configure fails rather than silently
  producing a reduced native Windows build when any of them is absent.
- `omalloc` is disabled because Win64 uses 32-bit `long` values and 64-bit
  pointers; the regular allocator is used instead.
- Existing optional dynamic modules default to disabled on MinGW.  They can be
  enabled individually while their dependencies and Windows behavior are
  validated.
- `ESingular` and `TSingular`, POSIX pipe/fork transports, networked SSI links,
  and debugger-driven external editing are not built or report a clear
  unsupported-operation error.

The repository's `Windows native build (UCRT64) - windows-latest` GitHub
Actions workflow repeats this build on a `windows-latest` host, checks that the
installed executable has no `msys-2.0.dll` or `cygwin1.dll` dependency,
assembles the portable runtime bundle, removes MSYS2 from `PATH`, and runs a
small Singular calculation from PowerShell.

## Signing and Windows warnings

A portable ZIP or unpackaged Win32 executable does not require a
notarization step.  An unsigned public download can nevertheless trigger
Microsoft Defender SmartScreen's "unrecognized app" warning, and managed
enterprise systems may refuse to run it.  For public releases, Authenticode-
sign the executable and any installer with one consistent, trusted publisher
identity and timestamp the signatures.  Signing improves publisher identity,
although new binaries can still need time to acquire SmartScreen reputation.

The packaged dependency licenses must also be preserved.  In particular,
readline and NTL's GF2X dependency are GPL-3.0-or-later, so public Windows
bundles should use Singular's GPL version 3 licensing option.  The workflow
records exact package versions and collects the installed runtime license
files into `THIRD_PARTY_LICENSES`.  A public binary release must also make the
corresponding source for that exact Singular build and its bundled GPL/LGPL
dependencies available in the manner required by those licenses.
