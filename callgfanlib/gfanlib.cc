#include <Singular/mod2.h>
#ifdef HAVE_FANS
#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbpolytope.h>

template class gfan::Vector<gfan::Integer>;
template class gfan::Vector<gfan::Rational>;
template class gfan::Matrix<gfan::Integer>;
template class gfan::Matrix<gfan::Rational>;

extern "C" int mod_init(void* gfanlibsingular)
{
  bbcone_setup();
  bbfan_setup();
  bbpolytope_setup();
}
#endif
