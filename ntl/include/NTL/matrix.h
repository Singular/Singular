#ifndef NTL_matrix__H
#define NTL_matrix__H

#include <NTL/tools.h>
#include <NTL/vector.h>


// matrix templates


#define NTL_matrix_decl(T,vec_T,vec_vec_T,mat_T)  \
class mat_T {  \
public:  \
  \
   vec_vec_T _mat__rep;  \
   long _mat__numcols;  \
  \
  \
   mat_T() { _mat__numcols = 0; }  \
   mat_T(const mat_T& l__a);  \
   mat_T& operator=(const mat_T& l__a);  \
   ~mat_T() { }  \
  \
   mat_T(NTL_NNS INIT_SIZE_TYPE, long l__n, long l__m);  \
  \
   void kill();  \
  \
   void SetDims(long l__n, long l__m);  \
  \
   long NumRows() const { return _mat__rep.length(); }  \
   long NumCols() const { return _mat__numcols; }  \
  \
   vec_T& operator[](long l__i) { return _mat__rep[l__i]; }  \
   const vec_T& operator[](long l__i) const { return _mat__rep[l__i]; }  \
  \
   vec_T& operator()(long l__i) { return _mat__rep[l__i-1]; }  \
   const vec_T& operator()(long l__i) const { return _mat__rep[l__i-1]; }  \
  \
   T& operator()(long l__i, long l__j) { return _mat__rep[l__i-1][l__j-1]; }  \
   const T& operator()(long l__i, long l__j) const   \
      { return _mat__rep[l__i-1][l__j-1]; }  \
  \
  \
  \
   long position(const vec_T& l__a) const { return _mat__rep.position(l__a); } \
   long position1(const vec_T& l__a) const { return _mat__rep.position1(l__a); } \
  mat_T(mat_T& l__x, NTL_NNS INIT_TRANS_TYPE) :  \
    _mat__rep(l__x._mat__rep, NTL_NNS INIT_TRANS), _mat__numcols(l__x._mat__numcols) { }  \
};  \
 \
inline const vec_vec_T& rep(const mat_T& l__a)  \
   { return l__a._mat__rep; }  \
  \
void swap(mat_T& l__X, mat_T& l__Y); \
  \
void MakeMatrix(mat_T& l__x, const vec_vec_T& l__a);  \



#define NTL_eq_matrix_decl(T,vec_T,vec_vec_T,mat_T) \
long operator==(const mat_T& l__a, const mat_T& l__b); \
long operator!=(const mat_T& l__a, const mat_T& l__b); \



#define NTL_io_matrix_decl(T,vec_T,vec_vec_T,mat_T) \
NTL_SNS istream& operator>>(NTL_SNS istream&, mat_T&); \
NTL_SNS ostream& operator<<(NTL_SNS ostream&, const mat_T&);  \


#define NTL_matrix_impl(T,vec_T,vec_vec_T,mat_T)  \
mat_T::mat_T(const mat_T& l__a)  \
{  \
   _mat__numcols = 0;  \
   SetDims(l__a.NumRows(), l__a.NumCols());  \
   _mat__rep = l__a._mat__rep;  \
}  \
  \
mat_T& mat_T::operator=(const mat_T& l__a)  \
{  \
   SetDims(l__a.NumRows(), l__a.NumCols());  \
   _mat__rep = l__a._mat__rep;  \
   return *this;  \
}  \
  \
  \
mat_T::mat_T(NTL_NNS INIT_SIZE_TYPE, long l__n, long l__m)  \
{  \
   _mat__numcols = 0;  \
   SetDims(l__n, l__m);  \
}  \
  \
void mat_T::kill()  \
{  \
   _mat__numcols = 0;  \
   _mat__rep.kill();  \
}  \
  \
void mat_T::SetDims(long l__n, long l__m)  \
{  \
   if (l__n < 0 || l__m < 0)  \
      NTL_NNS Error("SetDims: bad args");  \
  \
   if (l__m != _mat__numcols) {  \
      _mat__rep.kill();  \
      _mat__numcols = l__m;  \
   }  \
        \
   long l__oldmax = _mat__rep.MaxLength();  \
   long l__i;  \
   _mat__rep.SetLength(l__n);  \
  \
   for (l__i = l__oldmax; l__i < l__n; l__i++)  \
      _mat__rep[l__i].FixLength(l__m);  \
}  \
     \
        \
void MakeMatrix(mat_T& l__x, const vec_vec_T& l__a)  \
{  \
   long l__n = l__a.length();  \
  \
   if (l__n == 0) {  \
      l__x.SetDims(0, 0);  \
      return;  \
   }  \
  \
   long l__m = l__a[0].length();  \
   long l__i;  \
  \
   for (l__i = 1; l__i < l__n; l__i++)  \
      if (l__a[l__i].length() != l__m)  \
         NTL_NNS Error("nonrectangular matrix");  \
  \
   l__x.SetDims(l__n, l__m);  \
   for (l__i = 0; l__i < l__n; l__i++)  \
      l__x[l__i] = l__a[l__i];  \
}  \
  \
void swap(mat_T& l__X, mat_T& l__Y)  \
{  \
   NTL_NNS swap(l__X._mat__numcols, l__Y._mat__numcols);  \
   swap(l__X._mat__rep, l__Y._mat__rep);  \
}  \
  \




   

#define NTL_eq_matrix_impl(T,vec_T,vec_vec_T,mat_T)  \
long operator==(const mat_T& l__a, const mat_T& l__b)  \
{  \
   if (l__a.NumCols() != l__b.NumCols())  \
      return 0;  \
  \
   if (l__a.NumRows() != l__b.NumRows())  \
      return 0;  \
  \
   long l__n = l__a.NumRows();  \
   long l__i;  \
  \
   for (l__i = 0; l__i < l__n; l__i++)  \
      if (l__a[l__i] != l__b[l__i])  \
         return 0;  \
  \
   return 1;  \
}  \
  \
  \
long operator!=(const mat_T& l__a, const mat_T& l__b)  \
{  \
   return !(l__a == l__b);  \
}  \




#define NTL_io_matrix_impl(T,vec_T,vec_vec_T,mat_T)  \
NTL_SNS istream& operator>>(NTL_SNS istream& l__s, mat_T& l__x)  \
{  \
   vec_vec_T l__buf;  \
   l__s >> l__buf;  \
   MakeMatrix(l__x, l__buf);  \
   return l__s;  \
}  \
  \
NTL_SNS ostream& operator<<(NTL_SNS ostream& l__s, const mat_T& l__a)  \
{  \
   long l__n = l__a.NumRows();  \
   long l__i;  \
   l__s << "[";  \
   for (l__i = 0; l__i < l__n; l__i++) {  \
      l__s << l__a[l__i]; \
      l__s << "\n"; \
   }  \
   l__s << "]";  \
   return l__s;  \
}  \




#endif
