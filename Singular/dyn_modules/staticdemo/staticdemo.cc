#include <kernel/mod2.h>

#include <Singular/mod_lib.h>
#include <Singular/tok.h>

class SModulFunctions;

#ifndef STATIC_VERSION
# error This is a demo static module. It is not supposed to be built dynamically...
#endif

#include <reporter/reporter.h>

extern "C" int SI_MOD_INIT(staticdemo)(SModulFunctions*){ PrintS("init of staticdemo\n"); return (MAX_TOK); }

