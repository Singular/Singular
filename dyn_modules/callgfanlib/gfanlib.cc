#include <kernel/mod2.h>

#include <bbcone.h>
#include <bbfan.h>
#include <bbpolytope.h>
#include <gitfan.h>
#include"Singular/ipid.h"

template class gfan::Vector<gfan::Integer>;
template class gfan::Vector<gfan::Rational>;
template class gfan::Matrix<gfan::Integer>;
template class gfan::Matrix<gfan::Rational>;

extern "C" int mod_init(SModulFunctions* p)
{
  bbcone_setup(p);
  bbfan_setup(p);
  bbpolytope_setup(p);
  gitfan_setup(p);
}
