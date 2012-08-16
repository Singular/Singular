#ifndef POLYMAKE_DOCUMENTATION_H
#define POLYMAKE_DOCUMENTATION_H

#include <kernel/mod2.h>
#include <Singular/mod2.h>

#ifdef HAVE_FANS

#include <polymake_conversion.h>

#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbpolytope.h>

#include <Singular/blackbox.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>
#include <Singular/ipid.h>

#include <string>

void init_polymake_help();

#endif
#endif
