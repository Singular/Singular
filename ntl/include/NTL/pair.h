
#ifndef NTL_pair__H
#define NTL_pair__H

#include <NTL/tools.h>

#define NTL_pair_decl(S,T,pair_S_T)  \
class pair_S_T {  \
public:  \
   S a;  \
   T b;  \
  \
   pair_S_T() { }  \
   pair_S_T(const pair_S_T& l__x) : a(l__x.a), b(l__x.b) { } \
   pair_S_T& operator=(const pair_S_T& l__x) { a = l__x.a; b = l__x.b; return *this; } \
   pair_S_T(const S& l__x, const T& l__y) : a(l__x), b(l__y) { }  \
   ~pair_S_T() { }  \
};  \
  \
inline pair_S_T cons(const S& l__x, const T& l__y) { return pair_S_T(l__x, l__y); } \




#define NTL_pair_eq_decl(S,T,pair_S_T)  \
inline long operator==(const pair_S_T& l__x, const pair_S_T& l__y)  \
   { return l__x.a == l__y.a && l__x.b == l__y.b; }  \
inline long operator!=(const pair_S_T& l__x, const pair_S_T& l__y) \
   { return !(l__x == l__y); }  \



// For compatability...
#define NTL_pair_impl(S,T,pair_S_T)


// For compatability...
#define NTL_pair_eq_impl(S,T,pair_S_T)




#endif
