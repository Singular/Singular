//$Id$
#ifndef RING_WRAP_HEADER
#define RING_WRAP_HEADER
#include "ring.h"
#include "grammar.h"
#include "subexpr.h"
#include "ipid.h"
#include <boost/intrusive_ptr.hpp>
#include "Number.h"
using namespace boost;
//typedef intrusive_ptr<ip_sring> Ring;
// inline void intrusive_ptr_add_ref(ring r){
//     r->ref++;
// }
// inline void intrusive_ptr_release(ring r){
//     r->ref--;
//     if (r->ref<=0) rDelete(r);
// }

//typedef boosRing
class Ring{
   public:
     intrusive_ptr<ip_sring> pimpl;
     Ring(ring r=currRing): pimpl(r){
      
     }
     Ring(const Ring& r2):pimpl(r2.pimpl){
        
       }
     ~Ring(){
      
     }
 
};
void export_ring();
#endif
