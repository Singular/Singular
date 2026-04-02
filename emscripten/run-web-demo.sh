#!/usr/bin/env bash

set -eux

mkdir -p web-example
cp etc/emscripten/web-template/* web-example
cp Singular/Singular.wasm Singular/Singular.js Singular/Singular.data web-example

cd web-example
python3 -m http.server 9999