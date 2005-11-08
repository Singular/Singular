#ifndef CCRING_WRAP_HEADER
#define CCRING_WRAP_HEADER
#include "ring.h"
#include "grammar.h"
#include "subexpr.h"
#include "ipid.h"
class Ring{
  public:
    ring pimpl;
    Ring(ring r=currRing): pimpl(r){
      if (r!=NULL)
        ++(r->ref);
    }
    Ring(const Ring& r2):pimpl(r2.pimpl){
      pimpl->ref++;
    }
    ~Ring(){
      if (pimpl!=NULL)
        --(pimpl->ref);
    }
    Ring & operator=(const Ring& r2){
      r2.pimpl->ref++;
      pimpl->ref--;
      pimpl=r2.pimpl;
    }
   Ring & operator=(ring r2){
      r2->ref++;
      pimpl->ref--;
      pimpl=r2;
    }

   };
#endif