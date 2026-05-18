Code to allow building Singular to WASM using Emscripten.

Files:

- `build.sh`: Run as `emscripten/build.sh` in root directory of Singular. Requires installation of 'autoconf-archive' 'mercurial'. Tested on emsdk 3.1.23.

- `web-template`: Web template with 'xterm-pty'.

- `wasm_patch.c`: Patch script.

After executing `build.sh`, use `run-web-demo.sh` to setup a working website.

Todo:

- Add cddlib (done), ntl (done), normaliz (done), 4ti2, polymake, TOPCOM (done).
- "normaliz.lib", "gfan.lib", "tropical.lib" are working. Most libraries should work but this requires testing.

Setup Instructions:

- install emscripten:
```
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.23
./emsdk activate 3.1.23
source ./emsdk_env.sh
```
- compile Singular to WASM
Fromm root directory of this git repository, run 
```
bash emscripten/build.sh
```
- build all.lib (optional)
Fromm root directory of this git repository, run 
```
bash emscripten/gen_all_lib.sh
```
- setup website:
Fromm root directory of this git repository, run 
```
bash emscripten/run-web-demo.sh
```