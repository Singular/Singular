//$Id: intvec_wrap.h,v 1.3 2005-09-21 14:11:20 bricken Exp $
#ifndef INTVEC_WRAP_HEADER
#define INTVEC_WRAP_HEADER
#include <vector>
#include "mod2.h"
#include "intvec.h"
void export_intvec();
class better_intvec: public std::vector<int>{
public:
  better_intvec(iterator first, 
	iterator last,
	const allocator_type& __a = allocator_type()):
    std::vector<int>(first,last,__a){
  }
  better_intvec(int n=0):std::vector<int>(n){}
  better_intvec(intvec& iv):std::vector<int>(iv.length()){
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
