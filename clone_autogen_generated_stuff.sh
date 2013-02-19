#!/bin/sh

[ -d libpolys ] || { echo "the subdirectory libpolys does not exist; this does not seem to be the directory the Singular root directory"; exit 1;}

git clone git@github.com:Singular/AUTOGEN_generated_stuff.git
