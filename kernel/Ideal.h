#ifndef IDEAL_CPP_HEADER
#define IDEAL_CPP_HEADER
//$Id: Ideal.h,v 1.1 2005-09-08 12:07:20 bricken Exp $
#include "Poly.h"
//base for ideals as well for modules

template <class poly_type> class IdealBase {
 protected:
  std::vector<poly_type> storage;
 public:
  typedef poly_type value_type;
  typedef typename std::vector<poly_type>::size_type size_type;
  typedef typename std::vector<poly_type>::iterator iterator;
  typedef typename std::vector<poly_type>::difference_type difference_type;
  typedef typename std::vector<poly_type>::allocator_type allocator_type;
 IdealBase(){
 }
 IdealBase(iterator first, 
	     iterator last,
	     const typename
	     std::vector<poly_type>::allocator_type& __a = allocator_type()):
   storage(first,last,__a)
   {
   
 }
 poly_type& operator[] (int n){
   return storage[n];
 }
 const poly_type& operator[](int n) const{
   return storage[n];
 }
 void push_back(const poly_type& p){
   storage.push_back(p);
 }
 void push_front(const poly_type& p){
   storage.push_front(p);
 }

 iterator begin(){
   return storage.begin();
 }
 iterator end(){
   return storage.end();
 }
 size_type size(){
   return storage.size();
 }
 iterator
   insert(iterator __position, const value_type& __x){
   return storage.insert(__position,__x);
 }
 iterator
   erase(iterator __position){
   return storage.erase(__position);
 }
 iterator
   erase(iterator __first, iterator __last){
   return storage.erase(__first,__last);
 }
 void insert(iterator __pos, iterator __first, iterator __last){
   return insert(__pos,__first,__last);
 }
};

class Ideal:
public IdealBase<Poly>{
 public:
  Ideal(){
  }
  Ideal(iterator first, 
	iterator last,
	const allocator_type& __a = allocator_type()):
    IdealBase<Poly>(first,last,__a){
  }
};
class Modul:
public IdealBase<Vector>{
};
#endif
