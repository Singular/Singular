



#include <kernel/mod2.h>

#include <bbcone.h>
#include <bbfan.h>
#include <bbpolytope.h>
#include <gitfan.h>

#include <Singular/ipid.h>
#include <Singular/mod_lib.h>

template class gfan::Vector<gfan::Integer>;
template class gfan::Vector<gfan::Rational>;
template class gfan::Matrix<gfan::Integer>;
template class gfan::Matrix<gfan::Rational>;

int SI_MOD_INIT(gfanlib)(SModulFunctions* p)
{
  bbcone_setup(p);
  bbfan_setup(p);
  bbpolytope_setup(p);
  gitfan_setup(p);
  return 0;
}

#ifndef EMBED_PYTHON
extern "C"
{ 
int mod_init(SModulFunctions* psModulFunctions)
{ 
  return SI_MOD_INIT(gfanlib)(psModulFunctions); 
}
}
#endif
