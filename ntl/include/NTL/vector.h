
#ifndef NTL_vector__H
#define NTL_vector__H

#include <NTL/tools.h>

struct _ntl_VectorHeader {
   long length;
   long alloc;
   long init;
   long fixed;
};

union _ntl_AlignedVectorHeader {
   _ntl_VectorHeader h;
   double x1;
   long x2;
   char *x3;
};

#define NTL_VECTOR_HEADER_SIZE (sizeof(_ntl_AlignedVectorHeader))

#define NTL_VEC_HEAD(p) (& (((_ntl_AlignedVectorHeader *) p)[-1].h))

struct _ntl_vector_placement {
   void *p;
};

inline _ntl_vector_placement _ntl_vector_placement_fn(void *p)
{
   _ntl_vector_placement x;
   x.p = p;
   return x;
}

inline void *operator new(NTL_SNS size_t, _ntl_vector_placement x) { return x.p; }

// All of this monkey business is to avoid possible clashes with
// a "placement new" operator which may or may not be defined
// in a standard header file....why wasn't this just built
// into the language to begin with?

#ifndef NTL_RANGE_CHECK
#define NTL_RANGE_CHECK_CODE 
#else
#define NTL_RANGE_CHECK_CODE if (l__i < 0 || !_vec__rep || l__i >= NTL_VEC_HEAD(_vec__rep)->length) RangeError(l__i);
#endif

// vectors are allocated in chunks of this size

#ifndef NTL_VectorMinAlloc
#define NTL_VectorMinAlloc (4)
#endif

// vectors are always expanded by at least this ratio

#ifndef NTL_VectorExpansionRatio
#define NTL_VectorExpansionRatio (1.2)
#endif

// controls initialization during input

#ifndef NTL_VectorInputBlock
#define NTL_VectorInputBlock 50
#endif


#define NTL_vector_default(T)   \
void BlockConstruct(T* l__p, long l__n)  \
{  \
   long l__i;  \
  \
   for (l__i = 0; l__i < l__n; l__i++)  \
      (void) new(_ntl_vector_placement_fn(&l__p[l__i])) T;  \
}  \
  \
void BlockDestroy(T* l__p, long l__n)  \
{  \
   long l__i;  \
  \
   for (l__i = 0; l__i < l__n; l__i++)  \
      l__p[l__i].~T();  \
}



#define NTL_vector_decl(T,vec_T)  \
class vec_T {  \
public:  \
   T *_vec__rep;  \
  \
   void RangeError(long l__i) const;  \
  \
   vec_T() { _vec__rep = 0; }  \
   vec_T(NTL_NNS INIT_SIZE_TYPE, long l__n) { _vec__rep = 0; SetLength(l__n); }  \
   vec_T(const vec_T& l__a) { _vec__rep = 0; *this = l__a; }     \
   vec_T& operator=(const vec_T& l__a);  \
   ~vec_T();  \
   void kill(); \
  \
   void SetLength(long l__n);  \
   void SetMaxLength(long l__n); \
   void FixLength(long l__n); \
   void QuickSetLength(long l__n) { NTL_VEC_HEAD(_vec__rep)->length = l__n; } \
  \
   long length() const { return (!_vec__rep) ?  0 : NTL_VEC_HEAD(_vec__rep)->length; }  \
   long MaxLength() const { return (!_vec__rep) ?  0 : NTL_VEC_HEAD(_vec__rep)->init; } \
   long allocated() const { return (!_vec__rep) ?  0 : NTL_VEC_HEAD(_vec__rep)->alloc; } \
   long fixed() const { return _vec__rep && NTL_VEC_HEAD(_vec__rep)->fixed; } \
  \
   T& operator[](long l__i)   \
   {  \
      NTL_RANGE_CHECK_CODE  \
      return _vec__rep[l__i];  \
   }  \
  \
   const T& operator[](long l__i) const \
   {  \
      NTL_RANGE_CHECK_CODE  \
      return _vec__rep[l__i];  \
   }  \
  \
   T& RawGet(long l__i)   \
   {  \
      return _vec__rep[l__i];  \
   }  \
  \
   const T& RawGet(long l__i) const \
   {  \
      return _vec__rep[l__i];  \
   }  \
  \
   T& operator()(long l__i) { return (*this)[l__i-1]; }  \
   const T& operator()(long l__i) const { return (*this)[l__i-1]; } \
   \
  \
   const T* elts() const { return _vec__rep; }  \
   T* elts() { return _vec__rep; }  \
         \
 \
   vec_T(vec_T& l__x, NTL_NNS INIT_TRANS_TYPE) { _vec__rep = l__x._vec__rep; l__x._vec__rep = 0; } \
   long position(const T& l__a) const;  \
   long position1(const T& l__a) const;  \
};  \
 \
void swap(vec_T& l__x, vec_T& l__y);  \
void append(vec_T& l__v, const T& l__a); \
void append(vec_T& l__v, const vec_T& l__w); \




#define NTL_io_vector_decl(T,vec_T)  \
NTL_SNS istream& operator>>(NTL_SNS istream&, vec_T&);  \
  \
NTL_SNS ostream& operator<<(NTL_SNS ostream&, const vec_T&);  \


#define NTL_eq_vector_decl(T,vec_T)  \
long operator==(const vec_T& l__a, const vec_T& l__b);  \
long operator!=(const vec_T& l__a, const vec_T& l__b);


#define NTL_vector_impl(T,vec_T) NTL_vector_default(T) NTL_vector_impl_plain(T,vec_T)  


#if (!defined(NTL_CLEAN_PTR))

#define NTL_vector_impl_position(T,vec_T) \
long vec_T::position(const T& l__a) const  \
{  \
   if (!_vec__rep) return -1;  \
   long l__num_alloc = NTL_VEC_HEAD(_vec__rep)->alloc;  \
   long l__num_init = NTL_VEC_HEAD(_vec__rep)->init;  \
   if (&l__a < _vec__rep || &l__a >= _vec__rep + l__num_alloc) return -1;  \
   long l__res = (&l__a) - _vec__rep;  \
   \
   if (l__res < 0 || l__res >= l__num_alloc ||   \
       _vec__rep + l__res != &l__a) return -1;  \
   \
   if (l__res >= l__num_init)  \
       NTL_NNS Error("position: reference to uninitialized object"); \
   return l__res;  \
}  \
  \
long vec_T::position1(const T& l__a) const  \
{  \
   if (!_vec__rep) return -1;  \
   long l__len = NTL_VEC_HEAD(_vec__rep)->length;  \
   if (&l__a < _vec__rep || &l__a >= _vec__rep + l__len) return -1;  \
   long l__res = (&l__a) - _vec__rep;  \
   \
   if (l__res < 0 || l__res >= l__len ||   \
       _vec__rep + l__res != &l__a) return -1;  \
   \
   return l__res;  \
}  \


#else

#define NTL_vector_impl_position(T,vec_T) \
long vec_T::position(const T& l__a) const  \
{  \
   if (!_vec__rep) return -1;  \
   long l__num_alloc = NTL_VEC_HEAD(_vec__rep)->alloc;  \
   long l__num_init = NTL_VEC_HEAD(_vec__rep)->init;  \
   long l__res;  \
   l__res = 0;  \
   while (l__res < l__num_alloc && _vec__rep + l__res != &l__a)  l__res++;  \
   if (l__res >= l__num_alloc) return -1;  \
   if (l__res >= l__num_init)  \
       NTL_NNS Error("position: reference to uninitialized object"); \
   return l__res;  \
}  \
 \
long vec_T::position1(const T& l__a) const  \
{  \
   if (!_vec__rep) return -1;  \
   long l__len = NTL_VEC_HEAD(_vec__rep)->length;  \
   long l__res;  \
   l__res = 0;  \
   while (l__res < l__len && _vec__rep + l__res != &l__a)  l__res++;  \
   if (l__res >= l__len) return -1;  \
   return l__res;  \
}  \


#endif

#define NTL_vector_impl_plain(T,vec_T)  \
 \
void vec_T::SetLength(long l__n)   \
{   \
   long l__m;  \
  \
   if (l__n < 0) {  \
      NTL_NNS Error("negative length in vector::SetLength");  \
   }  \
   if (NTL_OVERFLOW(l__n, sizeof(T), 0))  \
      NTL_NNS Error("excessive length in vector::SetLength"); \
      \
   if (_vec__rep && NTL_VEC_HEAD(_vec__rep)->fixed) {\
      if (NTL_VEC_HEAD(_vec__rep)->length == l__n) \
         return; \
      else \
         NTL_NNS Error("SetLength: can't change this vector's length"); \
   }  \
   if (l__n == 0) {  \
      if (_vec__rep) NTL_VEC_HEAD(_vec__rep)->length = 0;  \
      return;  \
   }  \
  \
   if (!_vec__rep) {  \
      l__m = ((l__n+NTL_VectorMinAlloc-1)/NTL_VectorMinAlloc) * NTL_VectorMinAlloc; \
      char *l__p = (char *) NTL_SNS_MALLOC(l__m, sizeof(T), sizeof(_ntl_AlignedVectorHeader)); \
      if (!l__p) {  \
	 NTL_NNS Error("out of memory in vector::SetLength()");  \
      }  \
      _vec__rep = (T *) (l__p + sizeof(_ntl_AlignedVectorHeader)); \
  \
      BlockConstruct(_vec__rep, l__n); \
  \
      NTL_VEC_HEAD(_vec__rep)->length = l__n;  \
      NTL_VEC_HEAD(_vec__rep)->init = l__n;  \
      NTL_VEC_HEAD(_vec__rep)->alloc = l__m;  \
      NTL_VEC_HEAD(_vec__rep)->fixed = 0;  \
   }  \
   else if (l__n <= NTL_VEC_HEAD(_vec__rep)->init) {  \
      NTL_VEC_HEAD(_vec__rep)->length = l__n;  \
   }  \
   else  {  \
      if (l__n > NTL_VEC_HEAD(_vec__rep)->alloc) {  \
         l__m = NTL_NNS max(l__n, long(NTL_VectorExpansionRatio*NTL_VEC_HEAD(_vec__rep)->alloc));  \
         l__m = ((l__m+NTL_VectorMinAlloc-1)/NTL_VectorMinAlloc) * NTL_VectorMinAlloc; \
         char *l__p = ((char *) _vec__rep) - sizeof(_ntl_AlignedVectorHeader); \
         l__p = (char *) NTL_SNS_REALLOC(l__p, l__m, sizeof(T), sizeof(_ntl_AlignedVectorHeader)); \
         if (!l__p) {  \
	    NTL_NNS Error("out of memory in vector::SetLength()");  \
         }  \
         _vec__rep = (T *) (l__p + sizeof(_ntl_AlignedVectorHeader)); \
	 NTL_VEC_HEAD(_vec__rep)->alloc = l__m;  \
      }  \
      BlockConstruct(_vec__rep + NTL_VEC_HEAD(_vec__rep)->init, l__n - NTL_VEC_HEAD(_vec__rep)->init); \
      NTL_VEC_HEAD(_vec__rep)->length = l__n;  \
      NTL_VEC_HEAD(_vec__rep)->init = l__n;  \
   }  \
}  \
 \
 \
void vec_T::SetMaxLength(long l__n) \
{ \
   long l__OldLength = length(); \
   SetLength(l__n); \
   SetLength(l__OldLength); \
} \
 \
void vec_T::FixLength(long l__n) \
{ \
   if (_vec__rep) NTL_NNS Error("FixLength: can't fix this vector"); \
   if (l__n < 0) NTL_NNS Error("FixLength: negative length"); \
   if (l__n > 0) \
      SetLength(l__n); \
   else { \
      char *l__p = (char *) NTL_SNS_MALLOC(0, sizeof(T), sizeof(_ntl_AlignedVectorHeader)); \
      if (!l__p) {  \
	 NTL_NNS Error("out of memory in vector::FixLength()");  \
      }  \
      _vec__rep = (T *) (l__p + sizeof(_ntl_AlignedVectorHeader)); \
  \
      NTL_VEC_HEAD(_vec__rep)->length = 0;  \
      NTL_VEC_HEAD(_vec__rep)->init = 0;  \
      NTL_VEC_HEAD(_vec__rep)->alloc = 0;  \
   } \
   NTL_VEC_HEAD(_vec__rep)->fixed = 1; \
} \
  \
vec_T& vec_T::operator=(const vec_T& l__a)  \
{  \
   long l__i, l__n;  \
   T *l__p;  \
   const T *l__ap;  \
  \
   l__n = l__a.length();  \
   SetLength(l__n);  \
   l__ap = l__a.elts();  \
   l__p = elts();  \
  \
   for (l__i = 0; l__i < l__n; l__i++)  \
      l__p[l__i] = l__ap[l__i];  \
   return *this;  \
}  \
       \
  \
vec_T::~vec_T()  \
{  \
   if (!_vec__rep) return;  \
   BlockDestroy(_vec__rep, NTL_VEC_HEAD(_vec__rep)->init); \
   NTL_SNS free(((char *) _vec__rep) - sizeof(_ntl_AlignedVectorHeader));  \
}  \
   \
void vec_T::kill()  \
{  \
   if (!_vec__rep) return;  \
   if (NTL_VEC_HEAD(_vec__rep)->fixed) NTL_NNS Error("can't kill this vector"); \
   BlockDestroy(_vec__rep, NTL_VEC_HEAD(_vec__rep)->init); \
   NTL_SNS free(((char *) _vec__rep) - sizeof(_ntl_AlignedVectorHeader));  \
   _vec__rep = 0; \
}  \
  \
void vec_T::RangeError(long l__i) const  \
{  \
   NTL_SNS cerr << "index out of range in vector: ";  \
   NTL_SNS cerr << l__i;  \
   if (!_vec__rep)  \
      NTL_SNS cerr << "(0)\n";  \
   else  \
      NTL_SNS cerr << "(" << NTL_VEC_HEAD(_vec__rep)->length << ")\n";  \
   abort();  \
}  \
  \
   NTL_vector_impl_position(T,vec_T) \
 \
void swap(vec_T& l__x, vec_T& l__y)  \
{  \
   T* l__t;  \
   long l__xf = l__x.fixed();  \
   long l__yf = l__y.fixed();  \
   if (l__xf != l__yf ||   \
       (l__xf && NTL_VEC_HEAD(l__x._vec__rep)->length != NTL_VEC_HEAD(l__y._vec__rep)->length))  \
      NTL_NNS Error("swap: can't swap these vectors");  \
   l__t = l__x._vec__rep;  \
   l__x._vec__rep = l__y._vec__rep;  \
   l__y._vec__rep = l__t;  \
} \
 \
void append(vec_T& l__v, const T& l__a)  \
{  \
   long l__l = l__v.length(); \
   if (l__l >= l__v.allocated()) {  \
      long l__pos = l__v.position(l__a);  \
      l__v.SetLength(l__l+1);  \
      if (l__pos != -1)  \
         l__v[l__l] = l__v.RawGet(l__pos);  \
      else  \
         l__v[l__l] = l__a;  \
   } \
   else { \
      l__v.SetLength(l__l+1);  \
      l__v[l__l] = l__a;  \
   } \
}  \
  \
void append(vec_T& l__v, const vec_T& l__w)  \
{  \
   long l__l = l__v.length();  \
   long l__m = l__w.length();  \
   long l__i;  \
   l__v.SetLength(l__l+l__m);  \
   for (l__i = 0; l__i < l__m; l__i++)  \
      l__v[l__l+l__i] = l__w[l__i];  \
}





#define NTL_io_vector_impl(T,vec_T)  \
NTL_SNS istream & operator>>(NTL_SNS istream& l__s, vec_T& l__a)   \
{   \
   vec_T l__ibuf;  \
   long l__c;   \
   long l__n;   \
   if (!l__s) NTL_NNS Error("bad vector input"); \
   \
   l__c = l__s.peek();  \
   while (NTL_NNS IsWhiteSpace(l__c)) {  \
      l__s.get();  \
      l__c = l__s.peek();  \
   }  \
   if (l__c != '[') {  \
      NTL_NNS Error("bad vector input");  \
   }  \
   \
   l__n = 0;   \
   l__ibuf.SetLength(0);  \
      \
   l__s.get();  \
   l__c = l__s.peek();  \
   while (NTL_NNS IsWhiteSpace(l__c)) {  \
      l__s.get();  \
      l__c = l__s.peek();  \
   }  \
   while (l__c != ']' && l__c != EOF) {   \
      if (l__n % NTL_VectorInputBlock == 0) l__ibuf.SetMaxLength(l__n + NTL_VectorInputBlock); \
      l__n++;   \
      l__ibuf.SetLength(l__n);   \
      if (!(l__s >> l__ibuf[l__n-1])) NTL_NNS Error("bad vector input");   \
      l__c = l__s.peek();  \
      while (NTL_NNS IsWhiteSpace(l__c)) {  \
         l__s.get();  \
         l__c = l__s.peek();  \
      }  \
   }   \
   if (l__c == EOF) NTL_NNS Error("bad vector input");  \
   l__s.get(); \
   \
   l__a = l__ibuf; \
   return l__s;   \
}    \
   \
   \
NTL_SNS ostream& operator<<(NTL_SNS ostream& l__s, const vec_T& l__a)   \
{   \
   long l__i, l__n;   \
  \
   l__n = l__a.length();  \
   \
   l__s << '[';   \
   \
   for (l__i = 0; l__i < l__n; l__i++) {   \
      l__s << l__a[l__i];   \
      if (l__i < l__n-1) l__s << " ";   \
   }   \
   \
   l__s << ']';   \
      \
   return l__s;   \
}   \

#define NTL_eq_vector_impl(T,vec_T) \
long operator==(const vec_T& l__a, const vec_T& l__b) \
{  \
   long l__n = l__a.length();  \
   if (l__b.length() != l__n) return 0;  \
   const T* l__ap = l__a.elts(); \
   const T* l__bp = l__b.elts(); \
   long l__i;  \
   for (l__i = 0; l__i < l__n; l__i++) if (l__ap[l__i] != l__bp[l__i]) return 0;  \
   return 1;  \
} \
long operator!=(const vec_T& l__a, const vec_T& l__b) \
{  return !(l__a == l__b); }

   


#endif

