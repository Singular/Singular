Code to allow building Singular to WASM using Emscripten.

Files:

- `build.sh`: Run as `emscripten/build.sh` in root directory of Singular. Requires installation of 'autoconf-archive' 'mercurial'. Tested on emsdk 3.1.23.

- `web-template`: Web template with 'xterm-pty'.

- `wasm_patch.c`: Patch script.

After executing `build.sh`, use `run-web-demo` to setup a working website.

Issue:

- Loading some libs like `LIB "matrix.lib";` does not work.
  
- gfanlib and normaliz does not work.

Todo:

- Add cddlib(required by gfanlib) (done), ntl (done), normaliz, 4ti2, polymake, TOPCOM.
