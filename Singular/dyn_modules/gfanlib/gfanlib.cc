#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "bbcone.h"
#include "bbfan.h"
#include "bbpolytope.h"
#include "tropical.h"

#include "Singular/ipid.h"
#include "Singular/mod_lib.h"


template class gfan::Vector<gfan::Integer>;
template class gfan::Vector<gfan::Rational>;
template class gfan::Matrix<gfan::Integer>;
template class gfan::Matrix<gfan::Rational>;

extern "C" int SI_MOD_INIT(gfanlib)(SModulFunctions* p)
{
  gfan::initializeCddlibIfRequired();
  bbcone_setup(p);
  bbfan_setup(p);
  bbpolytope_setup(p);
  tropical_setup(p);
  return MAX_TOK;
}

#endif
