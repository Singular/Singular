//$Id: ring_wrap.h,v 1.5 2005-09-21 07:12:48 bricken Exp $
#ifndef RING_WRAP_HEADER
#define RING_WRAP_HEADER
#include "ring.h"
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
    void set(){
      rChangeCurrRing(pimpl);
    }
};
void export_ring();
#endif
