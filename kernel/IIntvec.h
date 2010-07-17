#ifndef ITERATABLE_INTVEC_H
#define ITERATABLE_INTVEC_H
#include <vector>
#include <kernel/intvec.h>
class Intvec: public std::vector<int>{
public:
  Intvec(iterator first, 
	iterator last,
	const allocator_type& __a = allocator_type()):
    std::vector<int>(first,last,__a){
  }
  Intvec(int n=0):std::vector<int>(n){}
  Intvec(intvec& iv):std::vector<int>(iv.length()){
    int n=iv.length();
    for(int i=0;i<n;i++){
      (*this)[i]=iv[i];
    }
  }
  intvec* allocate_legacy_intvec_copy() const{
    int s=size();
    intvec* iv=new intvec(s);
    
    for(int i=0;i<s;i++){
      (*iv)[i]=(*this)[i];
    }
    return iv;
  }
};
#endif
