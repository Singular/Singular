#!/usr/bin/env bash

cd Singular/LIB
echo 'string _ALL_LIB_VERSION_ = "WASM-Port";' > all.lib
for file in *.lib; do
    if [ "$file" != "all.lib" ]; then
        echo "LIB \"$file\";" >> all.lib
    fi
done